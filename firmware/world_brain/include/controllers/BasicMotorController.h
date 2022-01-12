#include "IMotorController.h"

class BasicMotorController : public IMotorController {
    public:
        BasicMotorController(int leftSpeedPin, int leftDirAPin, int leftDirBPin, int rightSpeedPin, int rightDirAPin, int rightDirBPin);
        void stop() override;
        void setSpeed(double left, double right, int ticksLeft, int ticksRight, elapsedMillis time) override;

    private:
        int leftSpeedPin;
        int leftDirAPin;
        int leftDirBPin;
        int rightSpeedPin;
        int rightDirAPin;
        int rightDirBPin;

        int targetTicksLeftPerSec = 0;
        int targetTicksRightPerSec = 0;

        int errorLeft = 0;
        int errorRight = 0;

        int errorIntLeft = 0;
        int errorIntRight = 0;
};
