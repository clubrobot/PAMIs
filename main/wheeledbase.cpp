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
#include "Wheeledbase.h"
#include "WheelMotor.h"

namespace wb
{
    PositionController positionControl;
    VelocityController velocityControl;

    PID linVelPID;
    PID angVelPID;

    Odometry odometry;

    Codewheel codewheel_right(RIGHT_CODEWHEEL_A, RIGHT_CODEWHEEL_B);
    Codewheel codewheel_left(LEFT_CODEWHEEL_B,LEFT_CODEWHEEL_A);

    I2CDCDriver driver(LEFT_MOTOR_FORWARD_, LEFT_MOTOR_BACKWARD_, LEFT_MOTOR_PWM_,
                       RIGHT_MOTOR_FORWARD_, RIGHT_MOTOR_BACKWARD_, RIGHT_MOTOR_PWM_, DRIVER_STBY_);

    WheelMotor leftWheel(350);
    WheelMotor rightWheel(350);

    PurePursuit pure_pursuit;
    TurnOnTheSpot turn_on_the_spot;


    void init(i2c_dev_t* dev)
    {
        leftWheel.setWheelRadius(25.172188481);
        rightWheel.setWheelRadius(25.172188481);
        leftWheel.setConstant(0.5);
        rightWheel.setConstant(0.5);

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

        odometry.setCodewheels(codewheel_left, codewheel_right);
        odometry.setAxleTrack(80);
        odometry.setSlippage(0);

        velocityControl.setWheels(leftWheel, rightWheel);

        velocityControl.setPID(linVelPID, angVelPID);

        velocityControl.setAxleTrack(80);
        velocityControl.setMaxLinAcc(500);
        velocityControl.setMaxLinDec(500);
        velocityControl.setMaxAngAcc(3.14);
        velocityControl.setMaxAngDec(3.14);

        odometry.setTimestep(20);
        velocityControl.setTimestep(20);
        positionControl.setTimestep(20);


        velocityControl.setSpinShutdown(false);

        linVelPID.setTunings(4, 0, 0);
        linVelPID.setOutputLimits(-400, 400);

        angVelPID.setTunings(4, 0, 0);
        angVelPID.setOutputLimits(-18.0278 / 2, 18.0278 / 2);

        positionControl.setVelLimits(350, 10);
        positionControl.setPosThresholds(6, 0.1);

        odometry.enable();

        Wheeledbase::INIT(&driver, &leftWheel, &rightWheel, &codewheel_right, &codewheel_left, &velocityControl, &linVelPID, &angVelPID, &positionControl, &pure_pursuit, &turn_on_the_spot, &odometry);
    }

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xDelay = pdMS_TO_TICKS(10);


    #define SMOOTHING_FACTOR 0.2

    float smoothLinVel = 0;
    float smoothAngVel = 0;

    void task(void *pvParameters){
        for(;;) {
            // Update odometry
            if (odometry.update()){

                smoothLinVel = SMOOTHING_FACTOR * odometry.getLinVel() + (1 - SMOOTHING_FACTOR) * smoothLinVel;
                smoothAngVel = SMOOTHING_FACTOR * odometry.getAngVel() + (1 - SMOOTHING_FACTOR) * smoothAngVel;

                positionControl.setPosInput(*odometry.getPosition());
                velocityControl.setInputs(smoothLinVel, smoothAngVel);
            }
            // Compute trajectory
            if (positionControl.update())
            {
                float linVelSetpoint = positionControl.getLinVelSetpoint();
                float angVelSetpoint = positionControl.getAngVelSetpoint();
                velocityControl.setSetpoints(linVelSetpoint, angVelSetpoint);
            }
            // Integrate engineering control
#if ENABLE_VELOCITYCONTROLLER_LOGS
            if (velocityControl.update())
                controllerLogs.update();
#else
            velocityControl.update();
#endif // ENABLE_VELOCITYCONTROLLER_LOGS

            vTaskDelay(pdMS_TO_TICKS(10));

            //printf("%s:%lu:%d\n", "nom_variable", millis(), millis()%10000);

            const Position* p = odometry.getPosition();

            printf("%f, %f, %f\n", p->x, p->y, p->theta);
        }
    }

}
