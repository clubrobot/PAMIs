//
// Created by Romain Pouvreau on 23/04/2026.
//

#ifndef PAMIS_WHEELEDBASE_H
#define PAMIS_WHEELEDBASE_H

#include "PositionController.h"
#include "VelocityController.h"
#include "driver/gpio.h"
#include "i2cdev.h"

#define LEFT_CODEWHEEL_A GPIO_NUM_0
#define LEFT_CODEWHEEL_B GPIO_NUM_2
#define RIGHT_CODEWHEEL_A GPIO_NUM_1
#define RIGHT_CODEWHEEL_B GPIO_NUM_3

#define LEFT_MOTOR_FORWARD 1
#define LEFT_MOTOR_BACKWARD 0
#define LEFT_MOTOR_PWM 2
#define RIGHT_MOTOR_FORWARD 4
#define RIGHT_MOTOR_BACKWARD 5
#define RIGHT_MOTOR_PWM 6
#define DRIVER_STBY 8

namespace wb
{

    extern PositionController positionControl;
    extern VelocityController velocityControl;

    extern PID linVelPID;
    extern PID angVelPID;

    void init(i2c_dev_t* dev);
    void task(void *pvParameters);
}


#endif //PAMIS_WHEELEDBASE_H
