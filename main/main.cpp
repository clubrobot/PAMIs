#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char* TAG = "main";


static uint8_t s_led_state = 0;

extern "C" void app_main(void)
{
    /* Configure the peripheral according to the LED type */
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
