//
// Created by Romain Pouvreau on 23/04/2026.
//

#ifndef PAMIS_TELEPLOT_H
#define PAMIS_TELEPLOT_H

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "lwip/sockets.h"
#include "esp_timer.h"

class Teleplot {
public:
    static bool init(const char* ip, uint16_t port);

    // Contrôle du temps (Source inspiration)
    static void fige_temps();
    static void relache_temps();

    // Méthodes de log
    static void log(const std::string& name, float value);
    static void log_int(const std::string& name, int value);
    static void logXY(const std::string& name, float x, float y);
    static void log_text(const std::string& name, const std::string& text);

private:
    struct TeleplotMsg {
        char data[160];
    };

    static void taskLoop(void* pvParameters);
    static int64_t get_timestamp();

    static int m_sock;
    static struct sockaddr_in m_dest_addr;
    static QueueHandle_t m_msgQueue;

    // Gestion du temps figé
    static int64_t m_fixedTimestamp;
    static bool m_isTimeFixed;
    static SemaphoreHandle_t m_timeMutex;

    static const size_t UDP_MAX_PACKET = 1400;
    static const uint32_t FLUSH_INTERVAL_MS = 15;
};

#endif //PAMIS_TELEPLOT_H
