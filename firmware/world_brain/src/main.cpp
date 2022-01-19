#include "intercom.h"
#include "constants.h"
#include "controllers/BasicMotorController.h"

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
    double half = *robot_teta + phi / 2;
    
    *robot_x = *robot_x + dcenter * cos(half);
    *robot_y = *robot_y + dcenter * sin(half);
    *robot_teta = *robot_teta + phi;

    while(*robot_teta > PI) {
        *robot_teta -= 2 * PI;
    }

    while(*robot_teta < -PI) {
        *robot_teta += 2 * PI;
    }

    transform_approx[0] = (int) *robot_x;
    transform_approx[1] = (int) *robot_y;
    transform_approx[2] = (int) (*robot_teta * RAD_TO_DEG);
}

IMotorController* motorController;
IntervalTimer timer;
elapsedMillis time;

bool shouldRunLogic = false;
void logic() {
    shouldRunLogic = true;
}

void setup() {
    Intercom::init("robot_position_read", DEFAULT_INTERCOM_SPEED);

    pinMode(interruptPin_left, INPUT_PULLDOWN);
    pinMode(interruptPin_right, INPUT_PULLDOWN);
    pinMode(dirPin_left, INPUT_PULLDOWN);
    pinMode(dirPin_right, INPUT_PULLDOWN);

    attachInterrupt(digitalPinToInterrupt(interruptPin_left), interruptionLeft, RISING);
    attachInterrupt(digitalPinToInterrupt(interruptPin_right), interruptionRight, RISING);
    NVIC_SET_PRIORITY(IRQ_PORTA, 0);

    pinMode(motorPwm_left, OUTPUT);
    pinMode(motorPwm_right, OUTPUT);
    pinMode(motorDirA_left, OUTPUT);
    pinMode(motorDirB_left, OUTPUT);
    pinMode(motorDirA_right, OUTPUT);
    pinMode(motorDirB_right, OUTPUT);

    digitalWrite(motorPwm_left, LOW);
    digitalWrite(motorPwm_right, LOW);
    digitalWrite(motorDirA_left, LOW);
    digitalWrite(motorDirB_left, LOW);
    digitalWrite(motorDirA_right, LOW);
    digitalWrite(motorDirB_right, LOW);

    motorController = new BasicMotorController(1, 2, 3, 4, 5, 6);

    timer.begin(logic, 10*1000); // in microseconds, so 10ms
    time = 0;
}

const float constDist = PI * wheel_diameter / wheel_ticks_count;

int requested_position[3];
int* req_x = &requested_position[0];
int* req_y = &requested_position[1];
int* req_teta = &requested_position[2];

String last_received_topic;
int* last_received_position;
int last_received_length;

int distance(int xa, int ya, int xb, int yb) {
    return sqrt(pow(xa - xb, 2) + pow(ya - yb, 2));
}

double anglePi(double angle) {
    while(angle >= 2*PI) {
        angle -= 2 * PI;
    }

    while(angle < 0) {
        angle += 2 * PI;
    }

    return angle;
}

void loop() {
    if(shouldRunLogic) {
        shouldRunLogic = false;
        // run logic at a specific rate per second

        updatePosition(constDist * ticksLeft, constDist * ticksRight);
        Intercom::publish("robot_position", transform_approx, 3);

        if(Intercom::instantReceiveIntArray(&last_received_topic, last_received_position, &last_received_length)) {
            if(last_received_length == 3) {
                if(last_received_topic == "move_robot") {
                    requested_position[0] = last_received_position[0];
                    requested_position[1] = last_received_position[1];
                    requested_position[2] = last_received_position[2]; // send a negative rotation if not required
                } else if(last_received_topic == "force_robot_position") {
                    *robot_x = last_received_position[0];
                    *robot_y = last_received_position[1];
                    *robot_teta = last_received_position[2] * DEG_TO_RAD;
                }
            }
        }

        if(Intercom::instantReceiveInt(&last_received_topic, &last_received_length)) {
            // last_received_length will contain the received int value, it's not really a length...
            if(last_received_topic == "rotate_robot") {
                requested_position[2] = last_received_length;
            }
        }

        int dist = distance(transform_approx[0], transform_approx[1], requested_position[0], requested_position[1]);
        if(dist > ARRIVAL_THRESHOLD) {
            double angle = atan2(requested_position[1] - transform_approx[1], requested_position[0] - transform_approx[0]) - transform_approx[2];
            // angle = anglePi(angle);

            double cos_angle = cos(angle);
            double sin_angle = sin(angle);

            motorController->setSpeed(MOTORS_V * (cos_angle + MOTORS_K * sin_angle), MOTORS_V * (cos_angle - MOTORS_K * sin_angle));
        } else {
            int requested_angle = requested_position[2];

            // if we want a specific orientation when arrived
            // as this is a request from a raspberry, values are integers and cannot be stored in radians
            if(requested_angle >= 0 && requested_angle < 360) {
                int current_angle = (int) (*robot_teta * RAD_TO_DEG);

                if(abs(current_angle - requested_angle) > ROTATION_THRESOLD_DEG) {
                    int direction = current_angle > requested_angle ? 1 : -1;
                    motorController->setSpeed(ROTATE_SPEED * direction, ROTATE_SPEED * -direction);
                } else {
                    motorController->setSpeed(0, 0);
                } else {
                    // our orientation is correct, we stop the motors
                    motorController->setSpeed(0, 0);
                }
            } else {
                // we don't want a specific orientation so we stop
                motorController->setSpeed(0, 0);
            }
        }

        ticksRight = 0;
        ticksLeft = 0;
    }
}