//
// Created by Romain Pouvreau on 23/04/2026.
//

#include "Teleplot.h"
#include <cstring>

int Teleplot::m_sock = -1;
struct sockaddr_in Teleplot::m_dest_addr;
QueueHandle_t Teleplot::m_msgQueue = nullptr;
int64_t Teleplot::m_fixedTimestamp = 0;
bool Teleplot::m_isTimeFixed = false;
SemaphoreHandle_t Teleplot::m_timeMutex = xSemaphoreCreateMutex();

bool Teleplot::init(const char* ip, uint16_t port) {
    if (m_msgQueue) return true;
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (m_sock < 0) return false;

    m_dest_addr.sin_addr.s_addr = inet_addr(ip);
    m_dest_addr.sin_family = AF_INET;
    m_dest_addr.sin_port = htons(port);

    m_msgQueue = xQueueCreate(100, sizeof(TeleplotMsg));
    xTaskCreate(taskLoop, "teleplot_task", 4096, NULL, 2, NULL);
    return true;
}

int64_t Teleplot::get_timestamp() {
    int64_t ts;
    xSemaphoreTake(m_timeMutex, portMAX_DELAY);
    ts = m_isTimeFixed ? m_fixedTimestamp : (esp_timer_get_time() / 1000);
    xSemaphoreGive(m_timeMutex);
    return ts;
}

void Teleplot::fige_temps() {
    xSemaphoreTake(m_timeMutex, portMAX_DELAY);
    m_fixedTimestamp = esp_timer_get_time() / 1000;
    m_isTimeFixed = true;
    xSemaphoreGive(m_timeMutex);
}

void Teleplot::relache_temps() {
    xSemaphoreTake(m_timeMutex, portMAX_DELAY);
    m_isTimeFixed = false;
    xSemaphoreGive(m_timeMutex);
}

// Formatage inspiré de la source : "nom:timestamp:valeur\n"
void Teleplot::log(const std::string& name, float value) {
    if (!m_msgQueue) return;
    TeleplotMsg msg;
    snprintf(msg.data, sizeof(msg.data), "%s:%lld:%.3f\n", name.c_str(), get_timestamp(), value);
    xQueueSend(m_msgQueue, &msg, 0);
}

void Teleplot::log_int(const std::string& name, int value) {
    if (!m_msgQueue) return;
    TeleplotMsg msg;
    snprintf(msg.data, sizeof(msg.data), "%s:%lld:%d\n", name.c_str(), get_timestamp(), value);
    xQueueSend(m_msgQueue, &msg, 0);
}

void Teleplot::logXY(const std::string& name, float x, float y) {
    if (!m_msgQueue) return;
    TeleplotMsg msg;
    // Format XY spécifique Teleplot : "nom:x;y:timestamp|xy\n" ou "nom:x:y:timestamp|xy\n"
    snprintf(msg.data, sizeof(msg.data), "%s:%.2f:%.2f:%lld|xy\n", name.c_str(), x, y, get_timestamp());
    xQueueSend(m_msgQueue, &msg, 0);
}

void Teleplot::log_text(const std::string& name, const std::string& text) {
    if (!m_msgQueue) return;
    TeleplotMsg msg;
    snprintf(msg.data, sizeof(msg.data), "%s:%lld:%s|t\n", name.c_str(), get_timestamp(), text.c_str());
    xQueueSend(m_msgQueue, &msg, 0);
}

void Teleplot::taskLoop(void* pvParameters) {
    char packetBuffer[UDP_MAX_PACKET];
    size_t currentLen = 0;
    TeleplotMsg incoming;

    while (true) {
        if (xQueueReceive(m_msgQueue, &incoming, pdMS_TO_TICKS(FLUSH_INTERVAL_MS))) {
            size_t msgLen = strlen(incoming.data);
            if (currentLen + msgLen >= UDP_MAX_PACKET) {
                sendto(m_sock, packetBuffer, currentLen, 0, (struct sockaddr *)&m_dest_addr, sizeof(m_dest_addr));
                currentLen = 0;
            }
            memcpy(packetBuffer + currentLen, incoming.data, msgLen);
            currentLen += msgLen;
        } else if (currentLen > 0) {
            sendto(m_sock, packetBuffer, currentLen, 0, (struct sockaddr *)&m_dest_addr, sizeof(m_dest_addr));
            currentLen = 0;
        }
    }
}