#include <cstdio>
#include "Codewheel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "i2cdev.h"
#include "sdkconfig.h"
#include "wheeledbase_.h"
#include "Teleplot.h"
#include "include/tasks.h"
#include "Wheeledbase.h"

#define PCA9685_ADDR 0x70
#define PCA9685_FREQ 1526
#define PCA9685_HIGH 4096
#define PCA9685_LOW 0
#define PCA9685_SDA GPIO_NUM_10
#define PCA9685_SCL GPIO_NUM_9

#include "sensors.h"
#include "vl53l7cx_api.h"
#include "Wheeledbase.h"

static const char* TAG = "main";

extern "C" void app_main(void)
{
    i2cdev_init();

    i2c_dev_t dev = {};
    dev.cfg.sda_io_num = PCA9685_SDA;
    dev.cfg.scl_io_num = PCA9685_SCL;
    dev.cfg.sda_pullup_en = GPIO_PULLUP_ENABLE;
    dev.cfg.scl_pullup_en = GPIO_PULLUP_ENABLE;
    dev.addr = PCA9685_ADDR;
    dev.cfg.master.clk_speed = 400000;
    dev.port = I2C_NUM_0;

    ESP_ERROR_CHECK(pca9685_init_desc(&dev, PCA9685_ADDR, I2C_NUM_0, GPIO_NUM_10, GPIO_NUM_9));

    //i2c_dev_create_mutex(&dev);

    wb::init(&dev);

    sensors::init(&dev);



    xTaskCreate(
                wb::task,
                "Wheeledbase loop",
                10000,
                nullptr,
                5,
                &tasks::hl_wb);


    xTaskCreate(
            sensors::task,
            "Sensors task",
            10000,
            nullptr,
            5,
            &tasks::hl_sensors);

    wb::positionControl.disable();
    wb::velocityControl.enable();



    for (;;)
    {

        wb::velocityControl.setSetpoints(300,0);
        vTaskDelay(500);
    }
}
