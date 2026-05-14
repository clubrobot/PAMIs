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

#define LEFT_MOTOR_FORWARD_ 1
#define LEFT_MOTOR_BACKWARD_ 0
#define LEFT_MOTOR_PWM_ 2
#define RIGHT_MOTOR_FORWARD_ 4
#define RIGHT_MOTOR_BACKWARD_ 5
#define RIGHT_MOTOR_PWM_ 6
#define DRIVER_STBY_ 8

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
