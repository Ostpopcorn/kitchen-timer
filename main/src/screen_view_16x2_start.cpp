#include "screen_view.h"
#include "string.h"
#include "esp_log.h"

#define TAG "SVIEW"

View16x2Start::View16x2Start()
{
}


View16x2Start::~View16x2Start()
{
}

void View16x2Start::update()
{
    
    std::string message = model->get_entry_string(ScreenModelEntry::ENTRY_STARTUP);
    write_text_on_screen(message,0,2,ViewBase::JUSTIFY_LEFT);

    if(has_backlight()){
        backlight->fade_to(127);
    }
}