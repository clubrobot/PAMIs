//
// Created by Romain Pouvreau on 16/04/2026.
//

#include "Odometry.h"


Position Odometry::m_pos = {0, 0, 0};
float Odometry::m_linVel = 0;
float Odometry::m_angVel = 0;
float Odometry::m_axleTrack = 0;
AbstractCodewheel* Odometry::m_leftCodewheel = nullptr;
AbstractCodewheel* Odometry::m_rightCodewheel = nullptr;
SemaphoreHandle_t Odometry::m_mutex = nullptr;
TaskHandle_t Odometry::m_taskHandle = nullptr;
float Odometry::m_slippage = 0;
int64_t Odometry::m_lastTimestamp = 0;

void Odometry::init(AbstractCodewheel& left, AbstractCodewheel& right, float axleTrack)
{
    m_leftCodewheel = &left;
    m_rightCodewheel = &right;
    m_axleTrack = axleTrack;

    m_mutex = xSemaphoreCreateMutex();

    if (m_mutex != nullptr)
    {
        m_lastTimestamp = esp_timer_get_time();
        m_leftCodewheel->restart();
        m_rightCodewheel->restart();

        xTaskCreate(taskLoop, "odom_task", 4096, nullptr, configMAX_PRIORITIES - 1, &m_taskHandle);
    }
}

Position Odometry::getPosition()
{
    Position p = {0, 0, 0};

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE)
    {
        p = m_pos;
        xSemaphoreGive(m_mutex);
    }
    return p;
}

void Odometry::setPosition(float x, float y, float theta)
{
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE)
    {
        m_pos = {x, y, theta};
        xSemaphoreGive(m_mutex);
    }
}

[[noreturn]] void Odometry::taskLoop(void* pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(ODOMETRY_TIMESTEP);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        vTaskDelayUntil(&xLastWakeTime, xDelay);

        const int64_t now = esp_timer_get_time();
        const float dt = static_cast<float>(now - m_lastTimestamp) / 1000000.0f; // Conversion µs en secondes
        m_lastTimestamp = now;

        if (dt > 0.0f)
        {
            process(dt);
        }
    }
}

void Odometry::process(const float dt) {
    const float dL = m_leftCodewheel->restart();
    const float dR = m_rightCodewheel->restart();

    const float deltaLinPos = (dL + dR) / 2.0f;
    const float deltaAngPos = (dR - dL) / m_axleTrack;
    const float deltaOrthLinPos = std::fabs(deltaLinPos) * m_slippage;

    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE) {

        const float avgTheta = m_pos.theta + (deltaAngPos / 2.0f);

        m_linVel = deltaLinPos / dt;
        m_angVel = deltaAngPos / dt;

        m_pos.x += deltaLinPos * cosf(avgTheta) - deltaOrthLinPos * sinf(avgTheta);
        m_pos.y += deltaLinPos * sinf(avgTheta) + deltaOrthLinPos * cosf(avgTheta);

        m_pos.theta += deltaAngPos;

        m_pos.theta = atan2f(sinf(m_pos.theta), cosf(m_pos.theta));

        xSemaphoreGive(m_mutex);
    }
}

float Odometry::getLinVel()
{
    float v = 0;
    if (xSemaphoreTake(m_mutex, portMAX_DELAY) == pdTRUE)
    {
        v = m_linVel;
        xSemaphoreGive(m_mutex);
    }
    return v;
}
