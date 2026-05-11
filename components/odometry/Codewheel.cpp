//
// Created by Romain Pouvreau on 16/04/2026.
//

#include "Codewheel.h"


Codewheel::Codewheel(int pin_a, int pin_b)
{
    //Initialisation des valeurs avant le load
    m_currentCounter = 0;
    m_startCounter = 0;
    m_wheelRadius = 1 / (2 * M_PI);
    m_countsPerRev = 1000;
    m_pin_a = pin_a;
    m_pin_b = pin_b;
}

void Codewheel::init()
{
    pcnt_unit_config_t unit_config = {};

    unit_config.low_limit = -PCNT_LIMIT;
    unit_config.high_limit = PCNT_LIMIT;

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &m_pcnt_unit));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(m_pcnt_unit, &filter_config));

    pcnt_chan_config_t chan_a_config = {};
    chan_a_config.edge_gpio_num = m_pin_a;
    chan_a_config.level_gpio_num = m_pin_b;

    pcnt_channel_handle_t pcnt_chan_a = nullptr;
    ESP_ERROR_CHECK(pcnt_new_channel(m_pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {};
    chan_b_config.edge_gpio_num = m_pin_b;
    chan_b_config.level_gpio_num = m_pin_a;

    pcnt_channel_handle_t pcnt_chan_b = nullptr;
    ESP_ERROR_CHECK(pcnt_new_channel(m_pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(m_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(m_pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(m_pcnt_unit));
}

void Codewheel::reset()
{
    m_startCounter = 0;
}

float Codewheel::restart()
{
    int rawCount = 0;
    pcnt_unit_get_count(m_pcnt_unit, &rawCount);
    float distance = (float)(rawCount - m_startCounter)
        / (float)m_countsPerRev
        * (2.0f * M_PI * m_wheelRadius);
    pcnt_unit_clear_count(m_pcnt_unit);
    m_startCounter = 0;
    m_currentCounter = 0;
    return distance;
}

float Codewheel::getTraveledDistance()
{
    int rawCount = 0;
    pcnt_unit_get_count(m_pcnt_unit, &rawCount);
    m_currentCounter = rawCount;
    return (float)(rawCount - m_startCounter)
        / (float)m_countsPerRev
        * (2.0f * M_PI * m_wheelRadius);
}

void Codewheel::setCountsPerRev(long countsPerRev)
{
    m_countsPerRev = countsPerRev;
}

void Codewheel::setWheelRadius(float wheelRadius)
{
    m_wheelRadius = wheelRadius;
}
