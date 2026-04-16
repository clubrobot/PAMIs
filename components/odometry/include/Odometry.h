//
// Created by Romain Pouvreau on 16/04/2026.
//

#ifndef PAMIS_ODOMETRY_H
#define PAMIS_ODOMETRY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "cmath"
#include "esp_timer.h"


#define ODOMETRY_TIMESTEP 10


/**
 * @brief Structure de position.
 * \author Ulysse Darmet
 * Position est une structure de Odometry.h qui permet en une variable d'obtenir la totalité des informations à propos du positionnement du robot.
 */
struct Position
{
    /**
     * @brief Constructeur de Position.
     * Constructeur de Position qui initialise la position au coordonnées (0,0) et à l'angle 0.
     */
    Position() : x(0), y(0), theta(0)
    {
    }

    /**
     * @brief  Constructeur de Position.
     *
     * Constructeur de Position qui initialise la position au coordonnées indiqués.
     *
     * @param x coordoonée en x initial.
     * @param y coordoonée en y initial.
     * @param theta angle initial.
     */
    Position(float x, float y, float theta) : x(x), y(y), theta(theta)
    {
    }

    static bool compare(const Position* one, const Position* two)
    {
        return one->x == two->x && one->y == two->y && one->theta == two->theta;
    }

    float x; /*!< Coordonnée en x.*/
    float y; /*!< Coordonnée en y.*/
    float theta; /*!< Angle.*/
};

/**
 * @brief Classe abstraite d'une roue codeuse.
 *
 * Cette classe est à implémenter pour être compatible avec la classe Odometry.
 * \author Ulysse Darmet
 */
class AbstractCodewheel
{
public:
    virtual ~AbstractCodewheel()
    {
    }

    /**
     * @brief Calcul la distance parcourue.
     *
     * Méthode à implémenter, dont le rôle est de retourner la distance parcourue depuis le dernier appel de cette méthode (ou depuis l'initialisation de l'objet).
     *
     * @return Distance parcourue depuis le dernier appel.
     */
    virtual float getTraveledDistance() = 0;
    /**
     * @brief Réinitialise l'objet.
     *
     * Réinitialise l'objet. C'est à dire, repasse tous les paramètres à leur état inital. De plus cette méthode dois retourner la distance parcourue depuis le dernier appel de AbstractCodewheel::getTraveledDistance.
     *
     * @return Distance parcourue depuis le dernier getter de distance.
     */
    virtual float restart() = 0;
};


class Odometry
{
public:

    Odometry() = delete;

    /**
    * @brief Defini les roues codeuses de Odometry.
    *
    * Paramètre les pointeurs sur les deux AbstractCodewheel ainsi que l'entraxe à utiliser pour le calcul d'odométrie.
    *
    * @param left AbstractCodewheel de la roue codeuse gauche.
    * @param right AbstractCodewheel de la roue codeuse droite.
    * @param axleTrack Entraxe en mm.
    */
    static void init(AbstractCodewheel& left, AbstractCodewheel& right, float axleTrack);

    /**
     * @brief Attribut une nouvelle position.
     *
     * A partir des coordonnées passer en paramètre, attribut les nouvelles coordonnées à sa Position.
     *
     * @param x Nouvelle coordonnée en x.
     * @param y Nouvelle coordonnée en y.
     * @param theta Nouvelle angle.
     */
    static void setPosition(float x, float y, float theta);

    /**
     * @brief Defini une nouvelle entraxe pour les roues codeuses.
     *
     * Change l'entraxe actuel par celui indiqué en paramètre.
     *
     * @param axleTrack Nouvelle entraxe en mm.
     */
    static void setAxleTrack(float axleTrack);

    /**
     * @brief Defini la nouvelle dérive orthogonal.
     *
     * Change la dérive orthogonal par celle indiquée en paramètre.
     *
     * @param slippage Nouvelle dérive orthogonal sans unité et signé.
     */
    static void setSlippage(float slippage);

    /**
     * @brief Retourne la position
     *
     * Retourne sa struc Position avec les dernières positions calculés.
     *
     * @return La structure Position.
     */
    static Position getPosition();

    /**
     * @brief Retourne la vitesse linéaire.
     *
     * Rend la dernière vitesse linéaire calculé.
     *
     * @return Vitesse lineaire en mm/s.
     */
    static float getLinVel();

    /**
     * @brief Retourne la vitesse angulaire.
     *
     * Rend la dernière vitesse angulaire calculé.
     *
     * @return Vitesse angulaire en rad/s.
     */
    static float getAngVel();

    /**
     * @brief Retourne l'entraxe utilisée.
     * @return Entraxe en mm.
     */
    static float getAxleTrack() { return m_axleTrack; }

    /**
     * @brief Retourne la dérive utilisée.
     * @return Dérive sans unité.
     */
    static float getSlippage() { return m_slippage; }

private:
    // La boucle de la tâche FreeRTOS
    [[noreturn]] static void taskLoop(void* pvParameters);
    static void process(float dt);          // dt en s
    static int64_t m_lastTimestamp;         // En microsecondes

    // Membres statiques
    static Position m_pos;
    static float m_linVel;
    static float m_angVel;
    static float m_axleTrack;
    static float m_slippage;

    static AbstractCodewheel* m_leftCodewheel;
    static AbstractCodewheel* m_rightCodewheel;

    // Mutex et Task Handle
    static SemaphoreHandle_t m_mutex;
    static TaskHandle_t m_taskHandle;
};


#endif //PAMIS_ODOMETRY_H
