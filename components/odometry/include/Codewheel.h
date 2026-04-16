//
// Created by Romain Pouvreau on 16/04/2026.
//

#ifndef PAMIS_CODEWHEEL_H
#define PAMIS_CODEWHEEL_H

#include "Odometry.h"
#include "driver/pulse_cnt.h"

#define PCNT_LIMIT 32767

class Codewheel : public AbstractCodewheel{

public:
    Codewheel(int pin_a, int pin_b);

    /**
     * @brief initialise les timers materiel
     */
    void init();

    /**
    * @brief renvoie le compteur de la roue codeuse
    * @return m_currentCounter
    */
    int getCounter(){update(); return m_currentCounter;}

    /**
     * @brief Donne le nombre de tics par tour courant.
     *
     * @return long Nombre de tics par tour.
     */
    int getCountsPerRev(){return m_countsPerRev;}

    /**
     * @brief Donne le rayon de la roue en mm
     *
     * @return float rayon en mm.
     */
    float getWheelRadius(){return m_wheelRadius;}

    /**
     * @brief Set le nombre de tics par tour.
     *
     * @param countsPerRev nb de tics par tour.
     */
    void setCountsPerRev(long countsPerRev);

    /**
     * @brief Set le rayon en mm de la roue.
     *
     * @param wheelRadius rayon en mm.
     */
    void setWheelRadius (float wheelRadius);

    /**
     * @brief Réinitialise le compteur à 0.
     *
     * Cette méthode peut rendre caduque le prochain Codewheel::getTraveledDistance.
     */
    void reset();

    /**
     * @brief Donne la distance parcouru.
     *
     * Cette méthode donne la distance parcouru par la roue depuis le dernier Codewheel::reset ou Codewheel::restart.
     * @return float
     */
    float getTraveledDistance() override;

    /**
     * @brief Donne la distance parcouru et reset le compteur.
     *
     * Cette méthode appel la méthode Codewheel::getTravemedDistance avant de mettre à jour le compteur de l'arduino (pas du compteur (puce elec)).
     *
     * @return float
     */
    float restart() override;

    /**
     * @brief Récupère le nombre de tics stoqués dans le compteur.
     *
     */
    void update();

protected:

    int m_currentCounter; //!< Tic courant.
    int m_startCounter;//!< Tic depuis le dernier reset

    float m_wheelRadius; //!< Rayon de la roue codeuse en  mm
    int m_countsPerRev;   //!<  Nombre de tics par tour de roue.

    pcnt_unit_handle_t m_pcnt_unit{};
    int m_pin_a;
    int m_pin_b;
};



#endif //PAMIS_CODEWHEEL_H
