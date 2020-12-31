#ifndef TIMER_SCREEN_H
#define TIMER_SCREEN_H
#include "LiquidCrystalGPIO.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "timer_class.h"
#include "time.h"


// template<class T>
class ScreenBase {
public:
    ScreenBase(){};
    virtual ~ScreenBase(){};

    typedef enum {
        CLOCK_BLANK = 0,
        CLOCK_WELCOME = 1,
        CLOCK_SHOW_TIMER = 10,
    } screen_views;
    virtual void change_view(screen_views new_view);
    // 
    virtual void update(Timer* timer = NULL);
protected:
    screen_views current_view{CLOCK_BLANK};

};

template<class T>
class Screen : public ScreenBase
{
    Screen();
    virtual ~Screen();
};

template<>
class Screen<LiquidCrystalGPIO*> : public ScreenBase
{
private:
    LiquidCrystalGPIO* lcd;
    gpio_num_t backlight_led = GPIO_NUM_NC;
    ledc_channel_config_t ledc_channel;
    ledc_timer_config_t ledc_timer;
    uint32_t fade_time = 1000;
    
public:

    Screen(LiquidCrystalGPIO* lcd);
    Screen(LiquidCrystalGPIO* lcd, gpio_num_t backlight_gpio);
    virtual ~Screen();
    void set_lcd_object(LiquidCrystalGPIO* lcd);

    virtual void change_view(screen_views new_view);

    virtual void update(Timer* timer = NULL) override;

    // Backlight functions
    void set_backlight_gpio(gpio_num_t gpio);
    void set_fade_time(uint32_t new_fade_time);
    void fade_backlight_to(uint32_t value);
};


#endif