#include "screen_view_16x2.h"
#include <string>
#include "esp_log.h"
#include "screen_model_entry_types.h"
#include "counter_clock.h"

#define TAG "SVIEW"

ButtonsController* View16x2ClockStop::button_controller{new ButtonsController{}};

RotaryEncoderController* View16x2ClockStop::rotary_encoder_controller{new RotaryEncoderController{}};

View16x2ClockStop::View16x2ClockStop()
{
    if(has_backlight()){
        backlight->fade_to(255);
    }
}

View16x2ClockStop::~View16x2ClockStop()
{
}

void View16x2ClockStop::update(bool redraw)
{
    write_text_on_screen("S",
                         0,0,ViewBase::JUSTIFY_CENTER);
    // Dela upp i "kör" och "står still" och "alarm"
    auto prmy_clk = model->get_entry(ENTRY_PRIMARY_TIMER);
    CounterClock* prmy_clk_data =prmy_clk->get<CounterClock>();

    if (prmy_clk_data && (redraw || prmy_clk->value_has_changed())) {
        write_text_on_screen(prmy_clk_data->to_string(' ',':'),
                            0,7,ViewBase::JUSTIFY_CENTER);
    }
    
    auto start = model->get_entry(ENTRY_START);
    std::string* start_data =start->get<std::string>();

    if (start_data && (redraw || start->value_has_changed())) {
        write_text_on_screen(*start_data,
                            1,4,ViewBase::JUSTIFY_CENTER);
    }
                         
                         
}