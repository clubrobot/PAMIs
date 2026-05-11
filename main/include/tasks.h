//
// Created by Romain Pouvreau on 10/05/2026.
//

#pragma once

#include "freertos/task.h"

namespace tasks
{
    inline TaskHandle_t  hl_wb = nullptr;
    inline TaskHandle_t  hl_sensors = nullptr;
}
