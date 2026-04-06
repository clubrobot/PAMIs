#include <cstdio>
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

#define PCNT_LIMIT 32767
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

    // Turn on the driver
    ESP_ERROR_CHECK(pca9685_set_pwm_value(&dev, DRIVER_STBY, PCA9685_HIGH));


    while (1)
    {
    }
}

void codewheels_tests(void *pvParameters)
{

    pcnt_unit_config_t right_unit_config = {};

    right_unit_config.low_limit = -PCNT_LIMIT;
    right_unit_config.high_limit = PCNT_LIMIT;

    pcnt_unit_config_t left_unit_config = {};

    left_unit_config.low_limit = -PCNT_LIMIT;
    left_unit_config.high_limit = PCNT_LIMIT;

    pcnt_unit_handle_t right_pcnt_unit = nullptr;
    pcnt_unit_handle_t left_pcnt_unit = nullptr;

    ESP_ERROR_CHECK(pcnt_new_unit(&right_unit_config, &right_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_new_unit(&left_unit_config, &left_pcnt_unit));

    // Set glitch filter
    pcnt_glitch_filter_config_t right_filter_config = {
        .max_glitch_ns = 1000,
    };

    pcnt_glitch_filter_config_t left_filter_config = {
        .max_glitch_ns = 1000,
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(right_pcnt_unit, &right_filter_config));
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(left_pcnt_unit, &left_filter_config));

    // Adding channels
    pcnt_chan_config_t right_channel_a_config = {};

    right_channel_a_config.edge_gpio_num = RIGHT_CODEWHEEL_A;
    right_channel_a_config.level_gpio_num = RIGHT_CODEWHEEL_B;

    pcnt_channel_handle_t right_pcnt_channel_a = nullptr;

    pcnt_chan_config_t left_channel_a_config = {};

    left_channel_a_config.edge_gpio_num = LEFT_CODEWHEEL_A;
    left_channel_a_config.level_gpio_num = LEFT_CODEWHEEL_B;

    pcnt_channel_handle_t left_pcnt_channel_a = nullptr;

    ESP_ERROR_CHECK(pcnt_new_channel(right_pcnt_unit, &right_channel_a_config, &right_pcnt_channel_a));
    ESP_ERROR_CHECK(pcnt_new_channel(left_pcnt_unit, &left_channel_a_config, &left_pcnt_channel_a));

    // Adding channels
    pcnt_chan_config_t right_channel_b_config = {};

    right_channel_b_config.edge_gpio_num = RIGHT_CODEWHEEL_B;
    right_channel_b_config.level_gpio_num = RIGHT_CODEWHEEL_A;

    pcnt_channel_handle_t right_pcnt_channel_b = nullptr;

    pcnt_chan_config_t left_channel_b_config = {};

    left_channel_b_config.edge_gpio_num = LEFT_CODEWHEEL_B;
    left_channel_b_config.level_gpio_num = LEFT_CODEWHEEL_A;

    pcnt_channel_handle_t left_pcnt_channel_b = nullptr;

    ESP_ERROR_CHECK(pcnt_new_channel(right_pcnt_unit, &right_channel_b_config, &right_pcnt_channel_b));
    ESP_ERROR_CHECK(pcnt_new_channel(left_pcnt_unit, &left_channel_b_config, &left_pcnt_channel_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(right_pcnt_channel_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(right_pcnt_channel_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(right_pcnt_channel_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(right_pcnt_channel_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(left_pcnt_channel_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(left_pcnt_channel_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(left_pcnt_channel_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(left_pcnt_channel_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(right_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(right_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(right_pcnt_unit));

    ESP_ERROR_CHECK(pcnt_unit_enable(left_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(left_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(left_pcnt_unit));

    // Report counter value
    int left_pulse_count = 0;
    int right_pulse_count = 0;

    while (1) {
        ESP_ERROR_CHECK(pcnt_unit_get_count(left_pcnt_unit, &left_pulse_count));
        ESP_ERROR_CHECK(pcnt_unit_get_count(right_pcnt_unit, &right_pulse_count));
        ESP_LOGI(TAG, "Pulse count (left, right): %d, %d", left_pulse_count, right_pulse_count);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static uint8_t s_led_state = 0;

extern "C" void app_main(void)
{

    // Init i2cdev library
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreatePinnedToCore(dc_test, TAG, configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(codewheels_tests, TAG, configMINIMAL_STACK_SIZE * 10, NULL, 5, NULL, 0);

    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);

    while (1)
    {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        gpio_set_level(GPIO_NUM_8, s_led_state);
        /* Toggle the LED state */
        s_led_state = !s_led_state;

        //vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
