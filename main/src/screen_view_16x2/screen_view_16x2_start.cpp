#include "screen_view_16x2.h"
#include "string.h"
#include "esp_log.h"
#include "screen_model_entry_types.h"
#include "battery_monitor.h"
#define TAG "SVIEW"

View16x2Start::View16x2Start()
{
}


View16x2Start::~View16x2Start()
{
}

void View16x2Start::update(bool redraw)
{
    
    auto start_text = model->get_entry<std::string>(ENTRY_STARTUP);
    if (redraw || start_text.value_has_been_updated()){
        write_text_on_screen(start_text.get_object(),0,2,ViewBase::JUSTIFY_LEFT);
    }

    write_text_on_screen("BAT:",
                        1,9,ViewBase::JUSTIFY_RIGHT);
                        
    auto bat_val{model->get_entry<BatteryDisplay>(ENTRY_BATTERY_VOLTAGE)};
    if(redraw || bat_val.value_has_been_updated()){
        write_text_on_screen(bat_val.get_object().to_string(),
                            1,10,ViewBase::JUSTIFY_LEFT);
    }
    write_text_on_screen("V",
                        1,14,ViewBase::JUSTIFY_LEFT);

    if(has_backlight()){
        backlight->fade_to(127);
    }
    // Add a animation, spinning wheel " | / - \ " so it spinns once per second
    // then hook update to a separate task. 

    anim.update();
    write_text_on_screen(anim.get_char_pointer(),
                        0,15,ViewBase::JUSTIFY_LEFT);

}