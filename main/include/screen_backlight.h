#ifndef TIMER_SCREEN_BACKLIGHT_H
#define TIMER_SCREEN_BACKLIGHT_H

#include "driver/gpio.h"
#include "driver/ledc.h"

class LcdBacklight
{
private:
    gpio_num_t backlight_led{GPIO_NUM_NC};
    ledc_channel_config_t ledc_channel;
    ledc_timer_config_t ledc_timer;
    uint32_t fade_time{1000};
    void set_gpio(gpio_num_t gpio);

public:
    LcdBacklight();
    LcdBacklight(gpio_num_t backlight_gipo);
    ~LcdBacklight();

    void set_fade_time(uint32_t new_fade_time);
    void fade_to(uint8_t value);
};


#endif
