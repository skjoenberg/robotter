#define MIN_DISTANCE 0.30
#define SPEED 0.2

void forward(Position2dProxy* pp);
bool obsFront(IrProxy* ir);
bool obsBack(IrProxy* ir);
bool obsLeft(IrProxy* ir);
bool obsRight(IrProxy* ir);
bool obsWeakLeft(IrProxy* ir);
bool obsWeakRight(IrProxy* ir);
void turnRight(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert);
void turnLeft(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert);
void reverse(Position2dProxy* pp, IrProxy* ir, PlayerClient* robert);
