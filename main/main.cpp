#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "i2cdev.h"
#include "pca9685.h"
#include "sdkconfig.h"

#define LEFT_MOTOR_FORWARD 0
#define LEFT_MOTOR_BACKWARD 1
#define LEFT_MOTOR_PWM 2
#define RIGHT_MOTOR_FORWARD 4
#define RIGHT_MOTOR_BACKWARD 5
#define RIGHT_MOTOR_PWM 6
#define DRIVER_STBY 8

#define PCA9685_ADDR 0x70
#define PCA9685_FREQ 1526
#define PCA9685_HIGH 4096
#define PCA9685_LOW 0
#define PCA9685_SDA GPIO_NUM_10
#define PCA9685_SCL GPIO_NUM_9

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
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, RIGHT_MOTOR_FORWARD, PCA9685_HIGH));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, LEFT_MOTOR_BACKWARD, PCA9685_LOW));
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, RIGHT_MOTOR_PWM, 2000));

    // Turn on the driver
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, DRIVER_STBY, PCA9685_HIGH));


    while (1)
    {
    }
}

static uint8_t s_led_state = 0;

extern "C" void app_main(void)
{

    // Init i2cdev library
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreatePinnedToCore(dc_test, TAG, configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL, 0);

    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        gpio_set_level(GPIO_NUM_8, s_led_state);
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
