//
// Created by Romain Pouvreau on 23/04/2026.
//


#include "PositionController.h"
#include "VelocityController.h"
#include "Odometry.h"
#include "wheeledbase.h"

#include "Codewheel.h"
#include "I2CDCDriver.h"
#include "Teleplot.h"
#include "WheelMotor.h"

namespace wb
{
    PositionController positionControl;
    VelocityController velocityControl;

    PID linVelPID;
    PID angVelPID;


    Codewheel codewheel_right(RIGHT_CODEWHEEL_A, RIGHT_CODEWHEEL_B);
    Codewheel codewheel_left(LEFT_CODEWHEEL_B,LEFT_CODEWHEEL_A);

    I2CDCDriver driver(LEFT_MOTOR_FORWARD_, LEFT_MOTOR_BACKWARD_, LEFT_MOTOR_PWM_,
                       RIGHT_MOTOR_FORWARD_, RIGHT_MOTOR_BACKWARD_, RIGHT_MOTOR_PWM_, DRIVER_STBY_);

    WheelMotor leftWheel(350);
    WheelMotor rightWheel(350);


    void init(i2c_dev_t* dev)
    {
        leftWheel.setWheelRadius(25.172188481);
        rightWheel.setWheelRadius(25.172188481);
        leftWheel.setConstant(0.3954);
        rightWheel.setConstant(0.3954);

        driver.init(dev);

        driver.attach(&leftWheel);
        driver.attach(&rightWheel);

        leftWheel.setVelocity(0);
        rightWheel.setVelocity(0);

        codewheel_right.init();

        codewheel_left.init();
        codewheel_right.setWheelRadius(25.172188481);

        codewheel_left.setWheelRadius(25.172188481);
        codewheel_left.setCountsPerRev(3840);
        codewheel_right.setCountsPerRev(3840);

        Odometry::init(codewheel_left, codewheel_right, 80);
        Odometry::setSlippage(0);

        velocityControl.setWheels(leftWheel, rightWheel);

        velocityControl.setPID(linVelPID, angVelPID);

        velocityControl.setAxleTrack(80);
        velocityControl.setMaxLinAcc(500);
        velocityControl.setMaxLinDec(500);
        velocityControl.setMaxAngAcc(3.14);
        velocityControl.setMaxAngDec(3.14);

        velocityControl.setSpinShutdown(false);

        linVelPID.setTunings(1, 0, 0);
        linVelPID.setOutputLimits(-400, 400);

        angVelPID.setTunings(1, 0, 0);
        angVelPID.setOutputLimits(-18.0278 / 2, 18.0278 / 2);

        positionControl.setVelLimits(350, 10);
        positionControl.setPosThresholds(6, 0.1);
        
    }

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(10);

    void task(void* pvParameters)
    {
        for (;;)
        {
            vTaskDelayUntil(&xLastWakeTime, xDelay);
            // Update odometry
            positionControl.setPosInput(Odometry::getPosition());

            velocityControl.setInputs(Odometry::getLinVel(), Odometry::getAngVel());

            if (positionControl.update())
            {
                float linVelSetpoint = positionControl.getLinVelSetpoint();
                float angVelSetpoint = positionControl.getAngVelSetpoint();
                velocityControl.setSetpoints(linVelSetpoint, angVelSetpoint);
            }
#if ENABLE_VELOCITYCONTROLLER_LOGS
            if (velocityControl.update())
                controllerLogs.update();
#else
            velocityControl.update();
#endif // ENABLE_VELOCITYCONTROLLER_LOGS
        }
    }
}
