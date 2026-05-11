//
// Created by Romain Pouvreau on 10/05/2026.
//

#ifndef PAMIS_SENSORS_H
#define PAMIS_SENSORS_H

#include "i2cdev.h"
#include "pca9685.h"

#define PCA9685_HIGH 4096
#define PCA9685_LOW 0

#define VL53L7CX_CHANNEL 11

namespace sensors
{
    esp_err_t init(i2c_dev_t* dev);
    [[noreturn]] void task(void *pvParameters);

};


#endif //PAMIS_SENSORS_H
