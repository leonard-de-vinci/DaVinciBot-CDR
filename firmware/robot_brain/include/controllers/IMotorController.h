class IMotorController {
    public:
        virtual void stop() = 0;
        virtual void setSpeed(double left, double right) = 0;
        virtual void updateOutput(int ticksLeft = 0, int ticksRight = 0, elapsedMillis time = 0) = 0;
        // in turns/s because the main program does the conversion from m/s
};
