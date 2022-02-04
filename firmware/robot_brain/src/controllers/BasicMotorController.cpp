#include <Arduino.h>
#include "controllers/BasicMotorController.h"
#include "constants.h"

#define kp 1
#define ki 0
#define kd 0

#define error_int_max 1000

BasicMotorController::BasicMotorController(int leftSpeedPin, int leftDirAPin, int leftDirBPin, int rightSpeedPin, int rightDirAPin, int rightDirBPin) {
    this->leftSpeedPin = leftSpeedPin;
    this->leftDirAPin = leftDirAPin;
    this->leftDirBPin = leftDirBPin;
    this->rightSpeedPin = rightSpeedPin;
    this->rightDirAPin = rightDirAPin;
    this->rightDirBPin = rightDirBPin;
}

void BasicMotorController::stop() {
    analogWrite(leftSpeedPin, 0);
    digitalWrite(leftDirAPin, HIGH);
    digitalWrite(leftDirBPin, LOW);

    analogWrite(rightSpeedPin, 0);
    digitalWrite(rightDirAPin, HIGH);
    digitalWrite(rightDirBPin, LOW);
}

void BasicMotorController::updateOutput(int ticksLeft, int ticksRight, elapsedMillis time) {
    int oldErrorLeft = errorLeft;
    int oldErrorRight = errorRight;

    errorLeft = targetTicksLeftPerSec - (ticksLeft * 1000 / time);
    errorRight = targetTicksRightPerSec - (ticksRight * 1000 / time);

    errorIntLeft += errorLeft;
    errorIntRight += errorRight;

    int _errorIntLeft = errorIntLeft * ki;
    int _errorIntRight = errorIntRight * ki;

    if(_errorIntLeft > error_int_max) {
        _errorIntLeft = error_int_max;
    } else if(_errorIntLeft < -error_int_max) {
        _errorIntLeft = -error_int_max;
    }

    if(_errorIntRight > error_int_max) {
        _errorIntRight = error_int_max;
    } else if(_errorIntRight < -error_int_max) {
        _errorIntRight = -error_int_max;
    }

    int cmdLeft = kp * errorLeft + _errorIntLeft + ki * _errorIntLeft + kd * (errorLeft - oldErrorLeft);
    int cmdRight = kp * errorRight + _errorIntRight + ki * _errorIntRight + kd * (errorRight - oldErrorRight);

    if(cmdLeft >= 0) {
        digitalWrite(leftDirAPin, HIGH);
        digitalWrite(leftDirBPin, LOW);
    } else {
        digitalWrite(leftDirAPin, LOW);
        digitalWrite(leftDirBPin, HIGH);
        cmdLeft = -cmdLeft;
    }

    if(cmdLeft > 1023)
        cmdLeft = 1023;

    analogWrite(leftSpeedPin, cmdLeft);

    if(cmdRight >= 0) {
        digitalWrite(rightDirAPin, HIGH);
        digitalWrite(rightDirBPin, LOW);
    } else {
        digitalWrite(rightDirAPin, LOW);
        digitalWrite(rightDirBPin, HIGH);
        cmdRight = -cmdRight;
    }

    if(cmdRight > 1023)
        cmdRight = 1023;

    analogWrite(rightSpeedPin, cmdRight);
}

void BasicMotorController::setSpeed(double left, double right) {
    targetTicksLeftPerSec = left * wheel_ticks_count;
    targetTicksRightPerSec = right * wheel_ticks_count;
}
