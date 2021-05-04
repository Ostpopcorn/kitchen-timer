#include "screen_backlight.h"

LcdBacklight::LcdBacklight()
{
}

LcdBacklight::LcdBacklight(gpio_num_t backlight_gipo)
{
    set_gpio(backlight_gipo);
}

LcdBacklight::~LcdBacklight()
{
}

void LcdBacklight::set_gpio(gpio_num_t gpio){
    backlight_led = gpio;
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL<<backlight_led),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // resolution of PWM duty
    ledc_timer.timer_num = LEDC_TIMER_1;            // timer index
    ledc_timer.freq_hz = 5000;                      // frequency of PWM signal
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;              // Auto select the source clock
    
    ledc_timer_config(&ledc_timer);

    ledc_channel.gpio_num   = gpio;
    ledc_channel.speed_mode = ledc_timer.speed_mode;
    ledc_channel.channel    = LEDC_CHANNEL_0;
    ledc_channel.timer_sel  = ledc_timer.timer_num;
    ledc_channel.duty       = target;
    ledc_channel.hpoint     = 0;
    ledc_channel.intr_type  = LEDC_INTR_DISABLE;

    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);

}

void LcdBacklight::set_fade_time(uint32_t new_fade_time){
    fade_time = new_fade_time;
}

void LcdBacklight::fade_to(uint8_t value){
    if(value != target){
        ledc_set_fade_with_time(ledc_channel.speed_mode,
                ledc_channel.channel, value, fade_time);
        ledc_fade_start(ledc_channel.speed_mode,
                ledc_channel.channel, LEDC_FADE_NO_WAIT);
        target = value;
    }else{
        // target value is the same as current so dont do anything.
    }
}