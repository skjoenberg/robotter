/*
 * ERSP Player driver
 */

/*
 * This driver is for interacting with the "ERSP" robots, made by Evolution
 * Robotics.
 *
 * This code is inspired by the player module of ER1 and P2OS; thanks to the
 * authors of these modules.
 */

#ifndef ERSP_ERSP_H
#define ERSP_ERSP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>  /* for abs() */

#include <libplayercore/playercore.h>

#ifdef HAVE_ERSP
#include <evolution/Resource.hpp>
#endif

#include <scorpion.h>

/*
 * Relevant types, macros, and constants.
 */

#define IR_MAX_RANGE			0.80
#define MAX_SENSOR_INTERVAL		1000
#define MOTOR_ACCELERATION		20
#define MOTOR_ANGULAR_ACCELERATION	(M_PI / 2.0) // radians/sec^2

#define ERSP2PLAYER_DISTANCE(dist)	((dist) / 100)
#define PLAYER2ERSP_DISTANCE(dist)	((dist) * 100)

#define ERSP2PLAYER_VELOCITY(vel)	((vel) / 100)
#define PLAYER2ERSP_VELOCITY(vel)	((vel) * 100)

#define ERSP2PLAYER_ACCELERATION(acc)	((acc) / 100)
#define PLAYER2ERSP_ACCELERATION(acc)	((acc) * 100)

// The main driver device interface data
typedef struct
{
	player_position2d_data_t position;
	player_bumper_data_t bumper;
	player_ir_data_t ir_range;
	player_bumper_data_t ir_binary;
	player_power_data_t power;
} __attribute__ ((packed)) player_ersp_data_t;

typedef struct
{
	player_devaddr_t devaddr;
	int subscriptions;
} player_ersp_dev_state_t;

typedef struct
{
	player_ersp_dev_state_t position;
	player_ersp_dev_state_t bumper;
	player_ersp_dev_state_t ir_range;
	player_ersp_dev_state_t ir_binary;
	player_ersp_dev_state_t power;
} player_ersp_state_t;

typedef struct
{
	double acceleration;
	double ang_acceleration;
	int sensor_interval;
} player_ersp_config_t;

class ERSP : public Driver
{
  private:

	////////////////////////////////////////////////////////////
	//	Generic driver
	////////////////////////////////////////////////////////////

	player_ersp_config_t ersp_config;
	player_ersp_state_t ersp_state;
	player_ersp_data_t ersp_data;
	bool running;

	// Handle incoming configuration messages.
	int HandleConfig(MessageQueue *queue, player_msghdr *hdr, void *data);

	// Handle incoming command messages.
	int HandleCommand(player_msghdr *hdr, void *data);

	// Publish state data.
	void PutData(player_ersp_state_t state, player_ersp_data_t data,
	             player_ersp_data_t prev);

	// Safely update subscription
	void UpdateSubscription(player_ersp_dev_state_t *devstate, int change,
	                        const char *name);

	////////////////////////////////////////////////////////////
	//	Evolution
	////////////////////////////////////////////////////////////

#ifdef HAVE_ERSP
	Evolution::ResourceManager *manager;
	Evolution::IResourceContainer *container;
#endif
	union {
#ifdef HAVE_ERSP
		Evolution::IDriveSystem *driver;
		Evolution::IAvoidance *avoid;
		Evolution::IOdometry *odometry;
		Evolution::IBattery *battery;
		Evolution::IBumpSensor *bumper;
		Evolution::IRangeSensor *range;
#endif
		void *handle;
	} devices[ERSP_DEVICES];
	const struct ersp_dev *devs;

	int SetupERSP();
	void set_motor_velocity(double velocity, double angular_velocity);
	void ResetRawPositions();

	////////////////////////////////////////////////////////////
	//	Driver interface
	////////////////////////////////////////////////////////////

  public:
	ERSP(ConfigFile *cf, int section);
	~ERSP(void);

	// Thread life-cycle
	void Main();
	int Setup();
	int Shutdown();

	// Message handling
	int Subscribe(player_devaddr_t id);
	int Unsubscribe(player_devaddr_t id);
	int ProcessMessage(MessageQueue *queue, player_msghdr *msghdr, void *data);
};

#endif
