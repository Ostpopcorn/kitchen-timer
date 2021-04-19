#include "screen_view_16x2.h"
#include <cstring>
#include "esp_log.h"
#include "screen_model_entry_types.h"

#define TAG "SVIEW"
View16x2ClockAlarm::View16x2ClockAlarm()
{
    if(has_backlight()){
        backlight->fade_to(255);
    }
}

View16x2ClockAlarm::~View16x2ClockAlarm()
{
}

void View16x2ClockAlarm::update()
{
    write_text_on_screen("A",
                         0,0,ViewBase::JUSTIFY_CENTER);
    // Dela upp i "kör" och "står still" och "alarm"
    write_text_on_screen(model->get_entry_object<std::string>(ENTRY_PRIMARY_TIMER),
                         0,7,ViewBase::JUSTIFY_CENTER);
    write_text_on_screen(model->get_entry_object<std::string>(ENTRY_START),
                         1,4,ViewBase::JUSTIFY_CENTER);
    write_text_on_screen(model->get_entry_object<std::string>(ENTRY_STOP),
                         1,11,ViewBase::JUSTIFY_CENTER);
                         
    // This should fade packlight up and down
}