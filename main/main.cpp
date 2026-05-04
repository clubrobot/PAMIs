#include <cstdio>

#include "Codewheel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "i2cdev.h"
#include "pca9685.h"
#include "sdkconfig.h"
#include "driver/pulse_cnt.h"

#define LEFT_MOTOR_FORWARD 0
#define LEFT_MOTOR_BACKWARD 1
#define LEFT_MOTOR_PWM 2
#define RIGHT_MOTOR_FORWARD 5
#define RIGHT_MOTOR_BACKWARD 4
#define RIGHT_MOTOR_PWM 6
#define DRIVER_STBY 8

#define PCA9685_ADDR 0x70
#define PCA9685_FREQ 1526
#define PCA9685_HIGH 4096
#define PCA9685_LOW 0
#define PCA9685_SDA GPIO_NUM_10
#define PCA9685_SCL GPIO_NUM_9

#define LEFT_CODEWHEEL_A GPIO_NUM_0
#define LEFT_CODEWHEEL_B GPIO_NUM_2
#define RIGHT_CODEWHEEL_A GPIO_NUM_1
#define RIGHT_CODEWHEEL_B GPIO_NUM_3

static const char* TAG = "main";

void dc_test(void *pvParameters)
{
    i2c_dev_t dev;
    memset(&dev, 0, sizeof(i2c_dev_t));

    // Initialise i2c
    dev.cfg.sda_io_num = PCA9685_SCL;
    dev.cfg.scl_io_num = PCA9685_SCL;
    dev.cfg.sda_pullup_en = GPIO_PULLUP_ENABLE;
    dev.cfg.scl_pullup_en = GPIO_PULLUP_ENABLE;

    // Initialise i2c pwm controller, pca9685
    ESP_ERROR_CHECK(pca9685_init_desc(&dev, PCA9685_ADDR, I2C_NUM_0, GPIO_NUM_10, GPIO_NUM_9));
    ESP_ERROR_CHECK(pca9685_init(&dev));
    ESP_ERROR_CHECK(pca9685_restart(&dev));

    ESP_ERROR_CHECK(pca9685_set_pwm_frequency(&dev, PCA9685_FREQ));


    // Turn the left wheel forward
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, LEFT_MOTOR_FORWARD, PCA9685_HIGH));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, LEFT_MOTOR_BACKWARD, PCA9685_LOW));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, LEFT_MOTOR_PWM, 2000));

    // Turn the right wheel forward
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, RIGHT_MOTOR_FORWARD, PCA9685_HIGH));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, RIGHT_MOTOR_BACKWARD, PCA9685_LOW));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, RIGHT_MOTOR_PWM, 2000));


    // Turn on the driver
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, DRIVER_STBY, PCA9685_HIGH));


    while (1)
    {
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

static uint8_t s_led_state = 0;


Codewheel codewheel_right(RIGHT_CODEWHEEL_A, RIGHT_CODEWHEEL_B);

Codewheel codewheel_left(LEFT_CODEWHEEL_B, LEFT_CODEWHEEL_A);

extern "C" void app_main(void)
{

    // Init i2cdev library
    ESP_ERROR_CHECK(i2cdev_init());

    //xTaskCreatePinnedToCore(dc_test, TAG, configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL, 0);
    //xTaskCreatePinnedToCore(codewheels_tests, TAG, configMINIMAL_STACK_SIZE * 10, NULL, 5, NULL, 0);

    codewheel_right.init();

    codewheel_left.init();
    codewheel_right.setWheelRadius(25.172188481);

    codewheel_left.setWheelRadius(25.172188481);
    codewheel_left.setCountsPerRev(3840);
    codewheel_right.setCountsPerRev(3840);

    Odometry::init(codewheel_left,codewheel_right,80);

    while(true){
        Position p = Odometry::getPosition();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        printf("%f,%f, %f \n",p.x,p.y, p.theta);
    }
}
