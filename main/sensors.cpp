//
// Created by Romain Pouvreau on 10/05/2026.
//

#include "sensors.h"

#include "esp_log.h"
#include "vl53l7cx_api.h"

static const char* TAG = "Sensors";
static i2c_master_bus_handle_t i2c_bus = nullptr;

static VL53L7CX_Configuration vl53l7cx_cfg;

namespace sensors
{
    esp_err_t init(i2c_dev_t *dev)
    {

        //Turn on the front sensor
        ESP_ERROR_CHECK(pca9685_set_pwm_value(dev, VL53L7CX_CHANNEL, PCA9685_HIGH));

        uint8_t status, is_alive;

        //Start the front sensor
        vl53l7cx_cfg.platform.address = VL53L7CX_DEFAULT_I2C_ADDRESS;
        i2cdev_get_shared_handle(I2C_NUM_0,(void**) &i2c_bus);

        i2c_device_config_t dev_cfg = {};
        dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        dev_cfg.device_address = (VL53L7CX_DEFAULT_I2C_ADDRESS >> 1); /* 0x52 -> 0x29 */
        dev_cfg.scl_speed_hz = dev->cfg.master.clk_speed;

        ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &dev_cfg, &vl53l7cx_cfg.platform.handle));


        status = vl53l7cx_is_alive(&vl53l7cx_cfg, &is_alive);
        if(!is_alive || status)
        {
            ESP_LOGE(TAG,"VL53L7CX not detected at requested address\n");
            return ESP_FAIL;
        }

        status = vl53l7cx_init(&vl53l7cx_cfg);
        if(status)
        {
            ESP_LOGE(TAG,"VL53L7CX ULD Loading failed\n");
            return ESP_FAIL;
        }

        ESP_LOGI(TAG,"VL53L7CX ULD ready ! (Version : %s)\n", VL53L7CX_API_REVISION);

        status |= vl53l7cx_set_resolution(&vl53l7cx_cfg, VL53L7CX_RESOLUTION_8X8);
        status |= vl53l7cx_set_ranging_frequency_hz(&vl53l7cx_cfg, 15);
        status |= vl53l7cx_start_ranging(&vl53l7cx_cfg);

        return status;
    }


    [[noreturn]] void task(void *pvParameters)
    {
        VL53L7CX_ResultsData results;
        uint8_t status, ready;
        for (;;)
        {
            status = vl53l7cx_check_data_ready(&vl53l7cx_cfg, &ready);

            if(ready)
            {
                vl53l7cx_get_ranging_data(&vl53l7cx_cfg, &results);

            }

            WaitMs(&(vl53l7cx_cfg.platform), 5);
        }
    }

};

