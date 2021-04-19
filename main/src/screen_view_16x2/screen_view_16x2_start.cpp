#include "screen_view_16x2.h"
#include "string.h"
#include "esp_log.h"
#include "screen_model_entry_types.h"

#define TAG "SVIEW"

View16x2Start::View16x2Start()
{
}


View16x2Start::~View16x2Start()
{
}

void View16x2Start::update()
{
    
    std::string message = model->get_entry_object<std::string>(ENTRY_STARTUP);
    write_text_on_screen(message,0,2,ViewBase::JUSTIFY_LEFT);

    write_text_on_screen("BAT:",
                        1,9,ViewBase::JUSTIFY_RIGHT);
    write_text_on_screen(model->get_entry_object<std::string>(ENTRY_BATTERY_VOLTAGE),
                        1,10,ViewBase::JUSTIFY_LEFT);
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