//
// Created by Romain Pouvreau on 23/04/2026.
//

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "esp_timer.h"


class Clock {
private:
    int64_t m_startTime; //!< Temps de référence en microsecondes (64 bits)

public:
    /**
     * @brief Constructeur de Clock
     * Initialise le marqueur de temps au moment de la création.
     */
    Clock() {
        m_startTime = esp_timer_get_time();
    }

    /**
     * @brief Récupère le temps écoulé depuis le dernier reset.
     * @return Temps écoulé en secondes (float).
     */
    float getElapsedTime();

    /**
     * @brief Reset le temps de référence.
     * @return Temps écoulé en secondes depuis le dernier reset.
     */
    float restart();
};

#endif // __CLOCK_H__

