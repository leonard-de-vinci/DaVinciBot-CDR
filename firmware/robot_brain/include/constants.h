#ifndef CONSTANTS_H
#define CONSTANTS_H

#define posInterruptPin_left 8
#define posInterruptPin_right 4
#define posDirPin_left 9
#define posDirPin_right 7

#define USE_SPEED_ENCODERS false
#define speedInterruptPin_left 0
#define speedInterruptPin_right 0
#define speedDirPin_left 0
#define speedDirPin_right 0

#define wheel_ticks_count 1024

// lengths are in mm
#define wheel_distance 275
#define wheel_diameter 61


#define motorPwm_left 22
#define motorPwm_right 23

#define motorDirA_left 33
#define motorDirB_left 34
#define motorDirA_right 35
#define motorDirB_right 36

#define ARRIVAL_THRESHOLD 8
#define ROTATION_THRESOLD_DEG 5
#define PT_FWD_THRESHOLD_RAD PI/4

#define MOTORS_V 2
#define MOTORS_K 1

#define ROTATE_SPEED 50

#define ROBOT_ID -1
#endif
