#include "intercom.h"
#include "constants.h"

#include <Arduino.h>
#include <math.h>


int ticksLeft = 0;
int ticksRight = 0;

double robot_transform[3];
double* robot_x = &robot_transform[0];
double* robot_y = &robot_transform[1];
double* robot_teta = &robot_transform[2];

int transform_approx[3];

void interruptionLeft() {
    if (digitalReadFast(dirPin_left)) {
        ticksLeft++;
    } else {
        ticksLeft--;
    }
}

void interruptionRight() {
    if (!digitalReadFast(dirPin_right)) {
        ticksRight++;
    } else {
        ticksRight--;
    }
}

void updatePosition(double dleft, double dright) {
    double dcenter = (dleft + dright) / 2;
    double phi = (dright - dleft) / wheel_distance;
    
    *robot_x = *robot_x + dcenter * cos(*robot_teta + phi / 2);
    *robot_y = *robot_y + dcenter * sin(*robot_teta + phi / 2);
    *robot_teta = *robot_teta + phi;

    ticksRight = 0;
    ticksLeft = 0;

    transform_approx[0] = (int) *robot_x;
    transform_approx[1] = (int) *robot_y;
    transform_approx[2] = (int) (*robot_teta * RAD_TO_DEG);
}

void setup() {
    Intercom::init("world_brain", 115200);

    pinMode(interruptPin_left, INPUT_PULLDOWN);
    pinMode(interruptPin_right, INPUT_PULLDOWN);
    pinMode(dirPin_left, INPUT_PULLDOWN);
    pinMode(dirPin_right, INPUT_PULLDOWN);

    attachInterrupt(digitalPinToInterrupt(interruptPin_left), interruptionLeft, RISING);
    attachInterrupt(digitalPinToInterrupt(interruptPin_right), interruptionRight, RISING);
    NVIC_SET_PRIORITY(IRQ_PORTA, 0);
}

const float constDist = PI * wheel_diameter / 1024;

void loop() {
    updatePosition(constDist * ticksLeft, constDist * ticksRight);
    Intercom::publish("robot_position", transform_approx, 3);

    delay(10);
}