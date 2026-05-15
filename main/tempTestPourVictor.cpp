//tester les encodeurs des moteurs

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Odometry.h"
#include "Codewheel.h"

extern "C" void app_main()
{
    //mettre numéro pin des encodeurs des roues
    Codewheel leftWheel(?, ?); //cf le header : (pin_a, pin_b)
    Codewheel rightWheel(?, ?); //(pin_a, pin_b)

    //rayon roue
    leftWheel.setWheelRadius(47/2); //rayon roue en mm (diamètre solidworks petite roue = 47mm)
    rightWheel.setWheelRadius(47/2);

    leftWheel.setCountsPerRev(1000);
    rightWheel.setCountsPerRev(1000);

    leftWheel.init();
    rightWheel.init();
    Odometry::init(leftWheel, rightWheel, 0.15f); //0.15f est le float de la distance entre les 2 roues, ici on a 15cm

    while (1)
    {
        Position pos = Odometry::getPosition();
        float linVel = Odometry::getLinVel(); //linVel, vitesse linéaire (avance/ recule) en m/s ?
        float angVel = Odometry::getAngVel(); //angVel, vitesse angulaire (tourne around the world) en rad/s ?

        printf("x=%.4f m  y=%.4f m  θ=%.4f rad  vLin=%.4f m/s  vAng=%.4f rad/s\n",
               pos.x, pos.y, pos.theta, linVel, angVel); //va afficher les float à 2 décimales après la virugule

        vTaskDelay(pdMS_TO_TICKS(100)); //pour afficher every 100 ms
    }
}