#include "IMotorController.h"

class BasicMotorController : public IMotorController {
    public:
        BasicMotorController(int leftSpeedPin, int leftDirAPin, int leftDirBPin, int rightSpeedPin, int rightDirAPin, int rightDirBPin);
        void stop() override;
        void setSpeed(double left, double right) override;
        void updateOutput(int ticksLeft = 0, int ticksRight = 0, elapsedMillis time = 0) override;

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
