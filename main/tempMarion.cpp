// 6 PAMIs + 1 PAMI ninja
/*Pour homologuer un  PAMI, avancer et s'arrêter à la détection d'un obstacle*/

/*déclancher un timer (clock) faire un while tant qu'on a pas tiré la tirette, détecter variable d'équipe et du n° du PAMI 
(7 en tout avec le PAMI ninja), attendre 80,5s avant de lancer les moteurs, il doit s'arreter à 100s (il a 20s pour se mettre dans le garde mangé)*/

//librairie sur les servo moteurs sur expressif idf



/*faire :  rm -rf build
           .  /home/marionlh/.espressif/v6.0/esp-idf/export.sh
           idf.py build    */
//ne pas oublier les sous modules : https://git-scm.com/book/fr/v2/Utilitaires-Git-Sous-modules

#include "components/clock/include/Clock.h"
#include "driver/gpio.h"
#include <cstdio>


//espace pour les GPIOs de la esp32
#define PIN_TIRETTE GPIO_?   //active qd ???
#define PIN_EQUIPE  GPIO_?   //jaune ou bleu
/*Pour le numéro du pami (il y aurait max 7 PAMIs donc 3bits (2^3-1))*/
#define PIN_PAMI_0  GPIO_?
#define PIN_PAMI_1  GPIO_?   
#define PIN_PAMI_2  GPIO_?
#define PIN_PAMI_Ninja  GPIO_?  

#define PIN_Servo   ????

#define T_ATTENTE   80.5f   //80.5s avant de lancer les moteurs
#define T_ARRET 100.0f  //100s arrêt total
#define T_ServoMoteur   90.0f //on se dit qu'à 90s (10s du début des PAMIs, le PAMI curseur bouge son servomoteur)

int calNumPami() //calcul du numéro du PAMI
{
    //on a 3 bits pour des PAMIs de 0 à 7
    int id = 0;
    id |= (gpio_get_level(PIN_PAMI_0) << 0) | gpio_get_level(PIN_PAMI_1) << 1) | gpio_get_level(PIN_PAMI_2) << 2);
    return id; //de 0 à 7 ça fait 8 PAMIs donc une des configs 3bits n'est pas utilisée
}

void lancerMoteurs()
{
    printf("3, 2, 1 ... action des moteurs !\n");
    // à remplir, regarder dans VelocityController
}

void arreterMoteurs()
{
    printf("Arrêt des 2 moteurs\n");
    // à remplir
}

voit initServomoteur()//https://components.espressif.com/components/espressif/servo/versions/0.1.0/readme
{
    //faire dans le terminal : idf.py add-dependency "espressif/servo^0.1.0"
        servo_config_t servo_cfg = {
        .max_angle = 180,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                SERVO_GPIO,
            },
            .ch = {
                LEDC_CHANNEL_0,
            },
        },
        .channel_number = 1,
    };

    // Initialize the servo
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg);

    //set the angle
    uint16_t angle = 0;
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle); 

}

void moveServomoteur() //https://docs.espressif.com/projects/esp-iot-solution/en/latest/motor/servo.html                       
{
    float angle = 150.0f; //set angle to 150 degree 
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
}

int stratPami(const numPami)  //stratégie des PAMI en fonction de leur num ET equipe
{
    switch (numPami)
    {
    case 0:
        a();
        break;
    case 1:
        a();
        break;
    case 2:
        b();
        break
    case 3:
        c();
        break;
    case 4:
        a();
        break;
    case 5:
        b();
        break
    default: //PAMI ninja (numéro 6 = 7e PAMI)
        d();
        break;
    }
    return 0;
}



extern "C" void app_main()
{
    //lire la config (équipe + ID PAMI)
    int equipe  = gpio_get_level(PIN_EQUIPE);
    int numPami  = calNumPami();

    printf("Équipe : %d, PAMI n°%d\n",equipe, numPami);

    //démarrage du clock
    Clock clock;
    clock.restart();   //t=0

    while (!(gpio_get_level(PIN_TIRETTE) == 0)) //à changer en fonction de qd active low ou high
    {
        vTaskDelay(pdMS_TO_TICKS(10)));    //lié à FreeRTOS, fonction pdMS_TO_TICKS utilisée dans Odometry.cpp  
                            //pour comprendre : https://freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay
    }

    clock.restart();    //on remet t=0, début attente des 80.5s
    printf("Départ du grand et beau robot, début attente PAMIs\n");

    bool vroumMoteurs = false; //au début, les moteurs ne sont pas lancés
    initServomoteur();
    
    while (1)
    {
        float t=clock.getElapsedTime(); //temps en s

        //A 80.5s
        if (!vroumMoteurs && t >= T_ATTENTE)
        {
            vroumMoteurs = true; //lancement des moteurs
            lancerMoteurs();
        }

        //A 90s
        if (t >= T_ServoMoteur)
        {
            moveServomoteur();
            printf("Curseur en mouvement ? il faut être bien placé\n");
        }

        //A 100s
        if (t >= T_ARRET)
        {
            arreterMoteurs();
            printf("FIN du match, PAMI hopefully dans le garde manger\n");
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}