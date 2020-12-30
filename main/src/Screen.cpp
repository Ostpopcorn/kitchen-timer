#include "Screen.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"


Screen<LiquidCrystalGPIO*>::Screen(LiquidCrystalGPIO* lcd){
    set_lcd_object(lcd);
}

Screen<LiquidCrystalGPIO*>::Screen(LiquidCrystalGPIO* lcd,gpio_num_t backlight_gpio){
    set_lcd_object(lcd);
    set_backlight_gpio(backlight_gpio);
}

template<class T>
Screen<T>::Screen(){

}

Screen<LiquidCrystalGPIO*>::~Screen(){
    if (lcd != NULL){
        free(lcd);
    }
    ledc_fade_func_uninstall();
}

template<class T>
Screen<T>::~Screen(){
}

void Screen<LiquidCrystalGPIO*>::set_lcd_object(LiquidCrystalGPIO* lcd){
    const int numRows = 2;
    const int numCols = 16;
    this->lcd = lcd;
    this->lcd->begin(numCols, numRows);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd->setCursor(0, 0);
    lcd->write('H');
    lcd->setCursor(1,0);
    lcd->write('E');
    lcd->setCursor(2, 0);
    lcd->write('J');
}

void Screen<LiquidCrystalGPIO*>::update(Timer* timer){
    int start_pos_col = 8;
    int start_pos_row = 0;
    if (lcd == NULL){
        ESP_LOGI("SCREEN","No lcd assigned");
        return;
    }
    char display_string[16];
    // size_t print_len = sprintf(display_string,"%.1f",timer->get_remainder_as_double(TIMER_0));
    size_t print_len = sprintf(display_string,"%s",timer->get_remainder_as_clock(TIMER_0).to_string(true,' ').c_str());

    for (size_t i = 0; i < print_len; i++)
    {
        lcd->setCursor(start_pos_col-i,start_pos_row);
        lcd->write(display_string[print_len-1-i]);
    }
    for (size_t i = print_len; i < 7; i++)
    {
        lcd->setCursor(start_pos_col-i,start_pos_row);
        lcd->write(' ');
    }
    
}

void Screen<LiquidCrystalGPIO*>::set_backlight_gpio(gpio_num_t gpio){
    backlight_led = gpio;
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
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;

    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);

}

void Screen<LiquidCrystalGPIO*>::set_fade_time(uint32_t new_fade_time){
    fade_time = new_fade_time;
}

void Screen<LiquidCrystalGPIO*>::fade_backlight_to(uint32_t value){
    ledc_set_fade_with_time(ledc_channel.speed_mode,
            ledc_channel.channel, value, fade_time);
    ledc_fade_start(ledc_channel.speed_mode,
            ledc_channel.channel, LEDC_FADE_NO_WAIT);

}