#ifndef TIMER_SCREEN_H
#define TIMER_SCREEN_H
#include "LiquidCrystalGPIO.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

class Screen{
    LiquidCrystalGPIO lcd;
    gpio_num_t backlight_led = GPIO_NUM_NC;
    uint32_t fade_time = 1000;
    ledc_channel_config_t ledc_channel;
    ledc_timer_config_t ledc_timer;
public:
    Screen();
    void set_backlight_gpio(gpio_num_t gpio);
    void set_fade_time(uint32_t new_fade_time);
    void fade_backlight_to(uint32_t value);
};


#endif