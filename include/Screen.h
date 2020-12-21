#ifndef TIMER_SCREEN_H
#define TIMER_SCREEN_H
#include "LiquidCrystalGPIO.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

template<class T>
class Screen{
    T* display_pointer;
public:
    Screen();
    Screen(LiquidCrystalGPIO* lcd);
    Screen(LiquidCrystalGPIO* lcd, gpio_num_t backlight_gpio);
    virtual ~Screen();
};

template<>
class Screen<LiquidCrystalGPIO>
{

    uint32_t fade_time = 1000;
    gpio_num_t backlight_led = GPIO_NUM_NC;
    ledc_channel_config_t ledc_channel;
    ledc_timer_config_t ledc_timer;
    void set_lcd_object(LiquidCrystalGPIO* lcd);
    void set_backlight_gpio(gpio_num_t gpio);
    void set_fade_time(uint32_t new_fade_time);
    void fade_backlight_to(uint32_t value);
};


#endif