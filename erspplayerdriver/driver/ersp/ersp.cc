/*
 * ERSP Player driver
 *
 * Copyright (C) 2007 Bue Petersen
 * Copyright (C) 2007 Jonas Fonseca
 * Copyright (C) 2007 Jesper Carlson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @ingroup drivers */
/** @{ */
/** @defgroup driver_ersp ersp
 * @brief Evolution ERSP-based robots

Many robots made by Evolution, such as the Scorpion robot, can be
controlled using the ERSP software package that provides access to the
underlying sensors and actuators. This driver maps the devices of an
ERSP-enabled robot to Player interfaces with equivalent functionality.

@par Compile-time dependencies

- The ERSP software package

@par Provides

The ERSP driver provides the following device interfaces, some of them
named:

- @ref interface_position2d
  - Controls the motor and drive system (if equipped).

- @ref interface_ir
  - This interface returns IR range sensor data (if equipped).

- "bumper" @ref interface_bumper
  - Returns data from bumper array (if equipped)

- "ir" @ref interface_bumper
  - Returns data from digital (hazardous) IR sensors (if equipped)

@par Supported configuration requests

- @ref interface_ir :
  - PLAYER_IR_POSE

@par Configuration file options

- acceleration (float) [sec^2]
  - Default: 20
- angular_acceleration (float) [rad / sec^2]
  - Default: PI / 2.0
- sensor_interval (integer) [msec]
  - Default: -1 (disabled)
  - Set to value > 0 to set interval between updates of sensor
    readings

@par Example

@verbatim
driver
(
	name "ersp"
 	plugin "libersp"
	provides [ "position2d:0" "bumper:::bumper:0" "ir:::bumper:1" "ir:0" "power:0]
	acceleration 0.2
	angular_acceleration 1.57
	sensor_interval 100
)
@endverbatim

@author Bue Petersen, Jonas Fonseca, Jesper Carlson
*/
/** @} */

#include "ersp.h"

#ifdef DEBUG
#define PLAYER_INFO(msg) PLAYER_MSG0(-1, msg)
#define PLAYER_DEBUG(msg) PLAYER_MSG0(-2, msg)
#else
#define PLAYER_INFO(msg)
#define PLAYER_DEBUG(msg)
#endif

/////////////////////////////////////////////////////
//	Driver module registration and initializaion
/////////////////////////////////////////////////////

Driver*
ERSP_Init(ConfigFile* cf, int section)
{
	return (Driver *) (new ERSP(cf, section));
}

void ERSP_Register(DriverTable* table)
{
	table->AddDriver("ersp", ERSP_Init);
}

/* Need the extern to avoid C++ name-mangling  */
extern "C" {

int player_driver_init(DriverTable* table)
{
	PLAYER_INFO("Registering ERSP driver.");
	ERSP_Register(table);

	return 0;
}

}

/////////////////////////////////////////////////////
//	Driver setup and shutdown
/////////////////////////////////////////////////////

ERSP::ERSP(ConfigFile* cf, int section)
		: Driver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
	// Zero device addresses and subscriptions, so that
	// we'll know later which interfaces were requested
	memset(&ersp_state, 0, sizeof(ersp_state));
	memset(&ersp_config, 0, sizeof(ersp_config));
	running = false;

	ersp_config.acceleration = cf->ReadFloat(section, "acceleration",
	                                         MOTOR_ACCELERATION);
	ersp_config.ang_acceleration = cf->ReadFloat(section, "angular_acceleration",
	                                             MOTOR_ANGULAR_ACCELERATION);
	ersp_config.sensor_interval = cf->ReadInt(section, "sensor_interval", -1);
	if (ersp_config.sensor_interval >= MAX_SENSOR_INTERVAL) {
		PLAYER_WARN1("Option 'sensor_interval' to big, resetting to %d",
		             MAX_SENSOR_INTERVAL);
		ersp_config.sensor_interval = MAX_SENSOR_INTERVAL;
	}

	if (cf->ReadDeviceAddr(&ersp_state.position.devaddr, section, "provides",
	                       PLAYER_POSITION2D_CODE, -1, NULL) == 0) {
		if (AddInterface(ersp_state.position.devaddr) != 0) {
			SetError(-1);
			return;
		}
	}

	if (cf->ReadDeviceAddr(&ersp_state.bumper.devaddr, section, "provides",
	                       PLAYER_BUMPER_CODE, -1, "bumper") == 0) {
		if (AddInterface(ersp_state.bumper.devaddr) != 0) {
			SetError(-1);
			return;
		}
	}

	if (cf->ReadDeviceAddr(&ersp_state.ir_range.devaddr, section, "provides",
	                       PLAYER_IR_CODE, -1, NULL) == 0) {
		if (AddInterface(ersp_state.ir_range.devaddr) != 0) {
			SetError(-1);
			return;
		}
	}

	if (cf->ReadDeviceAddr(&ersp_state.ir_binary.devaddr, section, "provides",
	                      PLAYER_BUMPER_CODE, -1, "ir") == 0) {
		if (AddInterface(ersp_state.ir_binary.devaddr) != 0) {
			SetError(-1);
			return;
		}
	}
	
	
	if (cf->ReadDeviceAddr(&ersp_state.power.devaddr, section, "provides",
	                      PLAYER_POWER_CODE, -1, NULL) == 0) {
		if (AddInterface(ersp_state.power.devaddr) != 0) {
			SetError(-1);
			return;
		}
	}
}

int ERSP::SetupERSP()
{
	static const struct ersp_dev ersp_devs[] = {

#define ERSP_INTERFACE(ROBOT, TYPE, NAME)
#define ERSP_INTERFACE_END
#define ERSP_DEV(ROBOT, ID, TYPE, NAME) \
	{ ROBOT##_##TYPE##_##ID, ROBOT##_##ID, ERSP_##TYPE, NAME },

#include ERSP_INCLUDE

#undef ERSP_INTERFACE
#undef ERSP_INTERFACE_END
#undef ERSP_DEV

	};
	devs = ersp_devs;

	if (running)
		return 0;

#ifdef HAVE_ERSP
	Evolution::Result result;

	// Get the ERSP resource manager
	manager = new Evolution::ResourceManager(NULL, &result);
	if (result != Evolution::RESULT_SUCCESS) {
		PLAYER_ERROR("Cannot create a resource manager.");
		return -1;
	}

	result = manager->get_resource_container(0, &container);
	if (result != Evolution::RESULT_SUCCESS) {
		PLAYER_ERROR("Cannot create a resource container.");
		return -1;
	}

	for (unsigned int i = 0; i < ERSP_DEVICES; i++) {
		/* XXX: Ordered by ersp_dev_type_t. */
		static const char *interfaces[] = {
			Evolution::IDriveSystem::INTERFACE_ID,
			Evolution::IAvoidance::INTERFACE_ID,
			Evolution::IOdometry::INTERFACE_ID,
			Evolution::IBattery::INTERFACE_ID,
			Evolution::IBumpSensor::INTERFACE_ID,
			Evolution::IBumpSensor::INTERFACE_ID,
			Evolution::IRangeSensor::INTERFACE_ID
		};
		void **handle = &devices[i].handle;
		const char *id = devs[i].id;
		const char *interface = interfaces[devs[i].type];

		result = container->obtain_interface(Evolution::NO_TICKET,
		                                     id, interface, handle);
		if (result != Evolution::RESULT_SUCCESS) {
			PLAYER_ERROR1("Cannot create a %s system.", id);
			return -1;
		}
	}

	devices[SCORPION_AVOID_AVOID].avoid->disable_avoidance(Evolution::NO_TICKET);
#endif /* HAVE_ERSP */

	return 0;
}

/* Should return 1 on error, and 0 on success. */
int ERSP::Setup()
{
	PLAYER_INFO("Driver setup");

	if (SetupERSP()) {
#ifdef HAVE_ERSP
		// FIXME: This might need to be changed so that
		// nothing is freed until the destructor is called
		// and instead have the SetupERSP conditionally
		// initialize variables.
		if (manager != NULL)
			delete manager;
#endif /* HAVE_ERSP */

		return 1;
	}

	// Now spawn reading thread
	// Prospone this to here since ::Shutdown will not be called if
	// an error condition is returned.
	StartThread();
	running = true;

	PLAYER_INFO("Driver setup done");

	return 0;
}

int ERSP::Shutdown()
{
	PLAYER_INFO("Driver shutdown");

	if (!running)
		return 0;

	StopThread();
	running = false;

	PLAYER_INFO("Driver has been shutdown");

	return 0;
}

ERSP::~ERSP (void)
{
#ifdef HAVE_ERSP
	/* Deallocate the Resource Manager.
	 *
	 * This means that the ERSP library will be initialized and ready
	 * throughout the life of the player server.
	 *
	 * FIXME: We still need to ensure that the various interfaces
	 * (driver, ...) are reset so the robot doesn't do anything. */

	if (manager != NULL)
		delete manager;
#endif /* HAVE_ERSP */
}

void
ERSP::Main()
{
#ifdef HAVE_ERSP
	Evolution::Timestamp timestamp;
	Evolution::Result result;
#endif /* HAVE_ERSP */
	player_ersp_data_t data, prev;
	player_ersp_state_t state;

	for(;;) {
		pthread_testcancel();

		Lock();

		// Save the previous data so we can check if new
		// sensor readings needs to be sent.
		prev = ersp_data;

		if(ersp_state.position.subscriptions != 0) {
			player_position2d_data_t *position = &ersp_data.position;
				
			memset(position, 0, sizeof(*position));
			
			for(unsigned int i=0; i < ERSP_DEVICES; i++)
			{
				
				if(devs[i].type != ERSP_ODOMETRY)
					continue;
#ifdef HAVE_ERSP
				//devices[i].odometry->reset_odometer(Evolution::NO_TICKET);
				Evolution::Pose2DT curr_position;
				result = devices[i].odometry->get_position (Evolution::NO_TICKET, &curr_position);
				if(result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR1("Error reading from odometry sensor %s", devs[i].id);
					SetError(-1);
				}
				//Odometric Pose
			
				position->pos.px = ERSP2PLAYER_DISTANCE(curr_position.x); 
				position->pos.py = ERSP2PLAYER_DISTANCE(curr_position.y);
				position->pos.pa= curr_position.theta;
				
				
#endif /* HAVE_ERSP */

				
			}
		}
		if (ersp_state.bumper.subscriptions != 0) {
			player_bumper_data_t *bumper = &ersp_data.bumper;

			memset(bumper, 0, sizeof(*bumper));

			for (unsigned int i = 0; i < ERSP_DEVICES; i++) {
				bool triggered = false;

				if (devs[i].type != ERSP_BUMP)
					continue;

#ifdef HAVE_ERSP
				result = devices[i].bumper->is_triggered(Evolution::NO_TICKET,
				                                         &timestamp, &triggered);
				if (result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR1("Error reading from binary sensor %s",
					              devs[i].id);
					SetError(-1);
				}
#endif /* HAVE_ERSP */

				bumper->bumpers[bumper->bumpers_count++] = (uint8_t) triggered == true;
			}
		}

		if (ersp_state.ir_range.subscriptions != 0) {
			player_ir_data_t *ir = &ersp_data.ir_range;

			memset(ir, 0, sizeof(*ir));

			for (unsigned int i = 0; i < ERSP_DEVICES; i++) {
				double distance = 0;

				if (devs[i].type != ERSP_IR_RANGE)
					continue;

#ifdef HAVE_ERSP
				result = devices[i].range->get_distance_reading(Evolution::NO_TICKET,
				                                                &timestamp, &distance);
				if (result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR1("Error reading from range sensor %s",
					              devs[i].id);
					SetError(-1);
				}
#endif /* HAVE_ERSP */

				distance = ERSP2PLAYER_DISTANCE(distance);
				if (distance > IR_MAX_RANGE) {
					distance = 2*IR_MAX_RANGE;
				}

				ir->ranges[ir->ranges_count++] = (float) distance;
			}
		}

		if (ersp_state.ir_binary.subscriptions != 0) {
			player_bumper_data_t *ir = &ersp_data.ir_binary;

			memset(ir, 0, sizeof(*ir));

			for (unsigned int i = 0; i < ERSP_DEVICES; i++) {
				bool triggered = false;

				if (devs[i].type != ERSP_IR_BINARY)
					continue;

#ifdef HAVE_ERSP
				result = devices[i].bumper->is_triggered(Evolution::NO_TICKET,
				                                         &timestamp, &triggered);
				if (result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR1("Error reading from binary sensor %s",
					              devs[i].id);
					SetError(-1);
				}
#endif /* HAVE_ERSP */

				ir->bumpers[ir->bumpers_count++] = (uint8_t) triggered == true;
			}
		}
		
		if (ersp_state.power.subscriptions != 0) {
			player_power_data_t *battery = &ersp_data.power;

			memset(battery, 0, sizeof(*battery));

			for (unsigned int i = 0; i < ERSP_DEVICES; i++) {
				double voltage = 0;

				if (devs[i].type != ERSP_BATTERY)
					continue;

#ifdef HAVE_ERSP
				result = devices[i].battery->get_percent_charge_left(Evolution::NO_TICKET,
				                                       &timestamp, &voltage);
				if (result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR1("Error reading from battery %s",
					              devs[i].id);
					SetError(-1);
				}
#endif /* HAVE_ERSP */

				battery->volts = (float) voltage;
				/*
				battery->watts =;
				battery->joules =;
				battery->percent =;
				battery->charging =;
				*/
				battery->valid = (PLAYER_POWER_MASK_VOLTS);
			}
		}
		
		
		data = ersp_data;
		state = ersp_state;
		Unlock();

		PutData(state, data, prev);

		// Handle pending messages
		if (!InQueue->Empty()) {
			PLAYER_DEBUG("Processing messages");
			ProcessMessages();
		}

		if (ersp_config.sensor_interval > 0)
			usleep(ersp_config.sensor_interval);
	}
}

/////////////////////////////////////////////////////
//	Message processing
/////////////////////////////////////////////////////

void
ERSP::set_motor_velocity(double velocity, double angular_velocity)
{
#ifdef DEBUG
	PLAYER_MSG4(2, "Setting motors: vel=%f, ang_vel=%f, acc=%f, ang_acc=%f",
	            velocity, angular_velocity,
	            ersp_config.acceleration, ersp_config.ang_acceleration);
#endif

#ifdef HAVE_ERSP
	Evolution::Result result;
	Evolution::IDriveSystem *driver = devices[SCORPION_DRIVE_DRIVE].driver;

	// Player uses m/sec while ERSP uses cm/s
	result = driver->move_and_turn(Evolution::NO_TICKET,
	                               PLAYER2ERSP_VELOCITY(velocity), PLAYER2ERSP_ACCELERATION(ersp_config.acceleration),
	                               angular_velocity, ersp_config.ang_acceleration);
	if (result != Evolution::RESULT_SUCCESS) {
		PLAYER_ERROR("Failed to set velocity");
		SetError(-1);
	}
#endif
}

void ERSP::ResetRawPositions()
{
#ifdef HAVE_ERSP
			Evolution::Result result;
			
			for(unsigned int i=0; i < ERSP_DEVICES; i++)
			{
				
				if(devs[i].type != ERSP_ODOMETRY)
					continue;
					
				result = devices[i].odometry->reset_odometer(Evolution::NO_TICKET);
				if(result != Evolution::RESULT_SUCCESS) {
					PLAYER_ERROR("Failed to reset odometry information");
					SetError(-1);
				}
				break;
			}
#endif
}


int
ERSP::ProcessMessage(MessageQueue *resp_queue, player_msghdr *hdr, void *data)
{
	// Look for configuration requests
	if (hdr->type == PLAYER_MSGTYPE_REQ)
		return HandleConfig(resp_queue, hdr, data);

	if (hdr->type == PLAYER_MSGTYPE_CMD)
		return HandleCommand(hdr, data);

	return -1;
}

int
ERSP::HandleConfig(MessageQueue *resp_queue, player_msghdr *hdr, void *data)
{
	// check for position config requests
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_IR_POSE,
	                          ersp_state.ir_range.devaddr)) {
		/* Return the bumper geometry. */
		if (hdr->size != 0) {
			PLAYER_WARN("Arg get ir range geom is wrong size; ignoring");
			return -1;
		}
		player_ir_pose_t ir_pose;

		memset(&ir_pose, 0, sizeof(ir_pose));
		ir_pose.poses[SCORPION_IR_BN_ENE].px = 0;

#define IR_POSE(id, _px, _py, _pa) \
		ir_pose.poses[SCORPION_##id].px = _px; \
		ir_pose.poses[SCORPION_##id].py = _py; \
		ir_pose.poses[SCORPION_##id].pa = _pa; \
		ir_pose.poses_count++;

		IR_POSE(IR_BN_ENE,  0.126,  0.092, -1.134);
		IR_POSE(IR_BN_WNW,  0.126, -0.092,  1.134);
		IR_POSE(IR_BN_N,    0.049,  0.000,  0);
		IR_POSE(IR_BN_NE,   0.092,  0.064, -0.611);
		IR_POSE(IR_BN_NW,   0.092, -0.064,  0.611);
		IR_POSE(IR_TE_NNW,  0.016, -0.173,  0.175);
		IR_POSE(IR_TE_NNE, -0.065, -0.169, -0.262);
		IR_POSE(IR_TW_NNW, -0.065,  0.169,  0.262);
		IR_POSE(IR_TW_NNE,  0.016,  0.173, -0.175);
		IR_POSE(IR_BS_W,   -0.172,  0.019,  1.571);
		IR_POSE(IR_BS_E,   -0.172, -0.019, -1.571);
		IR_POSE(IR_BW_S,   -0.077,  0.069,  3.054);
		IR_POSE(IR_BE_S,   -0.077, -0.069, -3.054)

		Publish(ersp_state.ir_range.devaddr, resp_queue,
		        PLAYER_MSGTYPE_RESP_ACK, PLAYER_IR_POSE,
		        &ir_pose, sizeof(ir_pose));

		return 0;
	}
	  else if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,
                                PLAYER_POSITION2D_REQ_RESET_ODOM,
                                ersp_state.position.devaddr))
  {
    /* reset position to 0,0,0: no args */
    if(hdr->size != 0)
    {
      PLAYER_WARN("Arg to reset position request is wrong size; ignoring");
      return(-1);
    }
    ResetRawPositions();
    //devices[i].odometry->reset_odometer(Evolution::NO_TICKET);
    this->Publish(ersp_state.position.devaddr, resp_queue,
                  PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_RESET_ODOM);
    return(0);
  }
	else {
		PLAYER_WARN("unknown config request to ersp driver");
		return -1;
	}
}

int
ERSP::HandleCommand(player_msghdr *hdr, void* data)
{
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD,
	                          PLAYER_POSITION2D_CMD_VEL,
	                          ersp_state.position.devaddr)) {

		player_position2d_cmd_vel_t cmd = *(player_position2d_cmd_vel_t*) data;

	// FIXME: check state: if false turn off the motors?
		set_motor_velocity(cmd.vel.px, cmd.vel.pa);
		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD,
	                               PLAYER_POSITION2D_CMD_CAR,
	                               ersp_state.position.devaddr)) {
		player_position2d_cmd_car_t cmd = *(player_position2d_cmd_car_t*) data;

		set_motor_velocity(cmd.velocity, cmd.angle);
		return 0;
	}

	return -1;
}


/////////////////////////////////////////////////////
//	Publish / Subscribe / Unsubscribe
/////////////////////////////////////////////////////

void
ERSP::UpdateSubscription(player_ersp_dev_state_t *devstate, int change,
                         const char *devname)
{
	// XXX: It is not possible to use locking here although driver
	// internals are accessed. It should, however, not matter that
	// much, since this code is the only writer, and it is really
	// just used as a hint.
	devstate->subscriptions += change;
	assert(devstate->subscriptions >= 0);
	PLAYER_MSG2(1, "%s %d subscriptions", devname, devstate->subscriptions);
}

int
ERSP::Subscribe(player_devaddr_t id)
{
	int result;

	// Do the subscription ...
	if ((result = Driver::Subscribe(id)) == 0) {
		// ... and increment the appropriate subscription counter
		if (Device::MatchDeviceAddress(id, ersp_state.position.devaddr)) {
			UpdateSubscription(&ersp_state.position, 1, "position");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.bumper.devaddr)) {
			UpdateSubscription(&ersp_state.bumper, 1, "bumper");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.ir_range.devaddr)) {
			UpdateSubscription(&ersp_state.ir_range, 1, "ir_range");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.ir_binary.devaddr)) {
			UpdateSubscription(&ersp_state.ir_binary, 1, "ir_binary");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.power.devaddr)) {
			UpdateSubscription(&ersp_state.power, 1, "battery");
		}
	}

	return result;
}

int
ERSP::Unsubscribe(player_devaddr_t id)
{
	int result;

	// Do the unsubscription ...
	if ((result = Driver::Unsubscribe(id)) == 0) {
		// ... and decrement the appropriate subscription counter
		if (Device::MatchDeviceAddress(id, ersp_state.position.devaddr)) {
			UpdateSubscription(&ersp_state.position, -1, "position");
			if (ersp_state.position.subscriptions == 0) {
				set_motor_velocity(0, 0);
			}
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.bumper.devaddr)) {
			UpdateSubscription(&ersp_state.bumper, -1, "bumper");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.ir_range.devaddr)) {
			UpdateSubscription(&ersp_state.ir_range, -1, "ir_range");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.ir_binary.devaddr)) {
			UpdateSubscription(&ersp_state.ir_binary, -1, "ir_binary");
		}
		else if (Device::MatchDeviceAddress(id, ersp_state.power.devaddr)) {
			UpdateSubscription(&ersp_state.power, -1, "battery");
		}
	}

	return result;
}

void
ERSP::PutData(player_ersp_state_t state, player_ersp_data_t data,
              player_ersp_data_t prev)
{
   if(state.position.subscriptions && 
        memcmp(&data.position, &prev.position, sizeof(data.position)))
        Publish(state.position.devaddr, NULL,
                PLAYER_MSGTYPE_DATA,
                PLAYER_POSITION2D_DATA_STATE,
                (void*)&(data.position),
                sizeof(player_position2d_data_t),
                NULL);
				
	if (state.bumper.subscriptions &&
	    memcmp(&data.bumper, &prev.bumper, sizeof(data.bumper)))
		Publish(state.bumper.devaddr, NULL,
		        PLAYER_MSGTYPE_DATA,
		        PLAYER_BUMPER_DATA_STATE,
		        (void *) &data.bumper,
		        sizeof(player_bumper_data_t),
		        NULL);

	if (state.ir_range.subscriptions &&
	    memcmp(&data.ir_range, &prev.ir_range, sizeof(data.ir_range)))
		Publish(state.ir_range.devaddr, NULL,
		        PLAYER_MSGTYPE_DATA,
		        PLAYER_IR_DATA_RANGES,
		        (void *) &data.ir_range,
		        sizeof(player_ir_data_t),
		        NULL);

	if (state.ir_binary.subscriptions &&
	    memcmp(&data.ir_binary, &prev.ir_binary, sizeof(data.ir_binary)))
		Publish(state.ir_binary.devaddr, NULL,
		        PLAYER_MSGTYPE_DATA,
		        PLAYER_BUMPER_DATA_STATE,
		        (void *) &data.ir_binary,
		        sizeof(player_bumper_data_t),
		        NULL);
	
	if (state.power.subscriptions &&
	    memcmp(&data.power, &prev.power, sizeof(data.power)))
		Publish(state.power.devaddr, NULL,
		        PLAYER_MSGTYPE_DATA,
		        PLAYER_POWER_DATA_STATE,
		        (void *) &data.power,
		        sizeof(player_power_data_t),
		        NULL);
}
