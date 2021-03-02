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
    
    std::string message = model->get_entry_object(ENTRY_STARTUP);
    write_text_on_screen(message,0,2,ViewBase::JUSTIFY_LEFT);

    write_text_on_screen("BAT:",
                        1,9,ViewBase::JUSTIFY_RIGHT);
    write_text_on_screen(model->get_entry_object(ENTRY_BATTERY_VOLTAGE),
                        1,10,ViewBase::JUSTIFY_LEFT);
    write_text_on_screen("V",
                        1,14,ViewBase::JUSTIFY_LEFT);

    if(has_backlight()){
        backlight->fade_to(127);
    }
}