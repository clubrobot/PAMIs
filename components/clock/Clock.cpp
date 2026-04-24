//
// Created by Romain Pouvreau on 23/04/2026.
//

#include "include/Clock.h"

float Clock::getElapsedTime()
{
    int64_t currentTime = esp_timer_get_time();
    // Calcul direct en microsecondes, puis conversion en secondes
    return static_cast<float>(currentTime - m_startTime) / 1000000.0f;
}

float Clock::restart()
{
    int64_t currentTime = esp_timer_get_time();
    float elapsedTimeInSeconds = static_cast<float>(currentTime - m_startTime) / 1000000.0f;
    m_startTime = currentTime;
    return elapsedTimeInSeconds;
}
