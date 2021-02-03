#include "screen_view_16x2.h"
#include "string.h"
#include "esp_log.h"
#define TAG "SVIEW"
View16x2SimpleClock::View16x2SimpleClock()
{
}

View16x2SimpleClock::~View16x2SimpleClock()
{
}

void View16x2SimpleClock::update()
{
    //
    // timer->get_remainder_as_clock(TIMER_0).to_string(true,' ').c_str()
    write_text_on_screen(model->get_entry_string(ScreenModelEntry::ENTRY_PRIMARY_TIMER),
                         0,7,ViewBase::JUSTIFY_CENTER);
    write_text_on_screen(model->get_entry_string(ScreenModelEntry::ENTRY_START),
                         1,4,ViewBase::JUSTIFY_CENTER);
    write_text_on_screen(model->get_entry_string(ScreenModelEntry::ENTRY_STOP),
                         1,11,ViewBase::JUSTIFY_CENTER);
                         
    if(has_backlight()){
        backlight->fade_to(255);
    }
    /*
    int start_offset = 4;
    int print_len_max = 8;
    int print_len = message.length()>print_len_max?print_len_max:message.length();
    if (memcmp(message.c_str(),&current_screen[start_offset],print_len)==0){
        ESP_LOGI(TAG,"O %s",current_screen);
        return; // message is allready on screen
    }
    ESP_LOGI(TAG,"Setting welcome message");
    //vTaskDelay(50 / portTICK_PERIOD_MS);
    for (size_t i = 0; i < print_len; i++)
    {
        lcd->setCursor(i+start_offset, 0);
        lcd->write(message[i]);
        current_screen[i+start_offset] = message[i]; 
    }
    ESP_LOGI(TAG,"U %s",current_screen);
    */
}