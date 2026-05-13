// 6 PAMIs + 1 PAMI ninja
/*Pour homologuer un  PAMI, avancer et s'arrêter à la détection d'un obstacle*/

/*déclancher un timer (clock) faire un while tant qu'on a pas tiré la tirette, détecter variable d'équipe et du n° du PAMI
(7 en tout avec le PAMI ninja), attendre 80,5s avant de lancer les moteurs, il doit s'arreter à 100s (il a 20s pour se mettre dans le garde mangé)*/

//librairie sur les servo moteurs sur expressif idf



/*faire :  rm -rf build
           .  /home/marionlh/.espressif/v6.0/esp-idf/export.sh
           idf.py build    */
//ne pas oublier les sous modules : https://git-scm.com/book/fr/v2/Utilitaires-Git-Sous-modules

#include "../components//clock/include/Clock.h"
#include "iot_servo.h"
#include "driver/gpio.h"
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//espace pour les GPIOs de la esp32
#define PIN_TIRETTE GPIO_NUM_24   //active qd pin à 0 (qd il y a la tirette pin à 1)
#define PIN_EQUIPE  GPIO_NUM_8   // = 0 qd jaune et = 1 qd bleu
/*Pour le numéro du pami (il y aurait max 7 PAMIs donc 3bits (2^3-1))*/
#define PIN_PAMI_0  GPIO_NUM_25
#define PIN_PAMI_1  GPIO_NUM_26
#define PIN_PAMI_2  GPIO_NUM_27

#define PIN_Servo1   GPIO_NUM_15
#define PIN_Servo2   GPIO_NUM_23

#define T_ATTENTE   85.5f   //85.5s avant de lancer les moteurs
#define T_ARRET 100.0f  //100s arrêt total
#define T_ServoMoteur   90.0f //on se dit qu'à 90s (10s du début des PAMIs, le PAMI curseur bouge son servomoteur)

int calNumPami() //calcul du numéro du PAMI
{
    //on a 3 bits pour des PAMIs de 0 à 7
    int id = 0;
    id |= (gpio_get_level(PIN_PAMI_0) << 0) | (gpio_get_level(PIN_PAMI_1) << 1) | (gpio_get_level(PIN_PAMI_2) << 2);
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

void initServomoteur()//https://components.espressif.com/components/espressif/servo/versions/0.1.0/readme
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
                PIN_Servo1,
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

void moveServomoteur(float angle) //https://docs.espressif.com/projects/esp-iot-solution/en/latest/motor/servo.html
{
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);
}

void zero_bleu()  { /* à faire */ }
void zero_jaune() { /* we can do it */ }
void one_bleu()  { /* à faire */ }
void one_jaune() { /* we can do it */ }
void two_bleu()  { /* à faire */ }
void two_jaune() { /* we can do it */ }
void trois_bleu()  { /* à faire */ }
void trois_jaune() { /* we can do it */ }
void vier_bleu()  { /* à faire */ }
void vier_jaune() { /* we can do it */ }
void five_bleu()  { /* à faire */ }
void five_jaune() { /* we can do it */ }
void ninja_bleu()  { /* à faire */ }
void ninja_jaune() { /* we can do it */ }

int stratPami(const int numPami, const int equipe)  //stratégie de déplacement des PAMI en fonction de leur num ET equipe
{
    switch (numPami)
    {
    case 0:
        if (equipe==0){  // 0 pour équipe jaune
        zero_bleu();}
        else{  // 1 équipe jaune
        zero_jaune();}
        break;
    case 1:
        if (equipe==0){
        one_bleu();}
        else{
        one_jaune();}
        break;
    case 2:
        if (equipe==0){
        two_bleu();}
        else{
        two_jaune();}
        break;
    case 3:
        if (equipe==0){
        trois_bleu();}
        else{
        trois_jaune();}
        break;
    case 4:
        if (equipe==0){
        vier_bleu();}
        else{
        vier_jaune();}
        break;
    case 5:
        if (equipe==0){
        five_bleu();}
        else{
        five_jaune();}
        break;
    default: //PAMI ninja (numéro 6 = 7e PAMI)
        if (equipe==0){
        ninja_bleu();}
        else{
        ninja_jaune();}
        break;
    }
    return 0;
}

extern "C" void app_main()
{
    gpio_reset_pin(PIN_TIRETTE);
    gpio_reset_pin(PIN_EQUIPE);
    gpio_reset_pin(PIN_PAMI_0);
    gpio_reset_pin(PIN_PAMI_1);
    gpio_reset_pin(PIN_PAMI_2);
    gpio_set_direction(PIN_TIRETTE, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_EQUIPE, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_PAMI_0, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_PAMI_1, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_PAMI_2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_TIRETTE, GPIO_PULLDOWN_ONLY);

    initServomoteur();
    //lire la config (équipe + ID PAMI)
    int equipe  = gpio_get_level(PIN_EQUIPE);
    int numPami  = calNumPami();

    printf("Équipe : %d, PAMI n°%d\n",equipe, numPami);

    //démarrage du clock
    Clock clock;
    clock.restart();   //t=0

    while (!(gpio_get_level(PIN_TIRETTE) == 0)) //à changer en fonction de qd active low ou high
    {
        vTaskDelay(pdMS_TO_TICKS(10));    //lié à FreeRTOS, fonction pdMS_TO_TICKS utilisée dans Odometry.cpp
                            //pour comprendre : https://freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/01-vTaskDelay
    }

    clock.restart();    //on remet t=0, début attente des 85.5s
    printf("Départ du grand et beau robot, début attente PAMIs\n");

    bool vroumMoteurs = false; //au début, les moteurs ne sont pas lancés
    bool tourneServo = false; //de mm, flag pour faire tourner l'actionneur


    while (1)
    {
        float t=clock.getElapsedTime(); //temps en s

        //A 80.5s
        if (!vroumMoteurs && t >= T_ATTENTE)
        {
            vroumMoteurs = true; //lancement des moteurs
            lancerMoteurs();
            stratPami(numPami, equipe);
        }

        //A 90s
        if (!tourneServo && t >= T_ServoMoteur)
        {
            tourneServo = true;
            moveServomoteur(0.0);
            printf("Curseur en mouvement ? il faut être bien placé\n");
        }

        //A 100s
        if (t >= T_ARRET)
        {
            arreterMoteurs();
            printf("FIN du match, PAMI hopefully dans le garde manger\n");
            moveServomoteur(0);
            vTaskDelay(pdMS_TO_TICKS(500));
            moveServomoteur(180);
            vTaskDelay(pdMS_TO_TICKS(500));

        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}