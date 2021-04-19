#ifndef ESP_TIMER_BATTERY_MONITOR_H
#define ESP_TIMER_BATTERY_MONITOR_H

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "string.h"
#include <functional>
#include "battery_display.h"

class Battery
{
public:
    typedef std::function<void(Battery*)> battery_callback_t;
    // typedef void (*battery_callback_t)(Battery*);
private:
    const int number_of_sample{10};
    const int default_vref{1100};
    const adc_unit_t unit{ADC_UNIT_1};
    const adc_atten_t atten{ADC_ATTEN_DB_11};
    const adc_bits_width_t width{ADC_WIDTH_BIT_12};
    const int voltage_divider_factor{2};
    esp_adc_cal_characteristics_t *adc_chars{NULL};

    adc1_channel_t analog_1_channel{ADC1_CHANNEL_MAX};
    gpio_num_t monitor_enable_pin{GPIO_NUM_NC};
    void enable_monitor();
    void disable_monitor();
    battery_callback_t callback{NULL};
    void set_pins(adc1_channel_t battery_analog_1_channel,gpio_num_t battery_monitor_enable);
    int last_measure{0};
    void set_last_mesurement(int);

public:
    Battery();
    Battery(adc1_channel_t battery_analog_1_channel,gpio_num_t battery_monitor_enable);
    ~Battery();
    void register_callback(battery_callback_t new_callback);
    void measure();
    int get_last_mesurement();
    BatteryDisplay get_last_mesurement_display() const;
    // std::string get_last_mesurement_as_string();
};


#endif