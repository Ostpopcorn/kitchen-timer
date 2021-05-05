#include "screen_view_16x2.h"
#include "string.h"
#include "esp_log.h"
#include "screen_model_entry_types.h"
#include "counter_clock.h"

#define TAG "SVIEW"


ButtonsController* View16x2ClockPause::button_controller{new ButtonsController{}};

View16x2ClockPause::View16x2ClockPause()
{
    if(has_backlight()){
        backlight->fade_to(255);
    }
}

View16x2ClockPause::~View16x2ClockPause()
{
}

void View16x2ClockPause::update(bool redraw)
{
    write_text_on_screen("P",
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

    auto stop = model->get_entry(ENTRY_STOP);
    std::string* stop_data =stop->get<std::string>();

    if (stop_data && (redraw || stop->value_has_changed())) {
        write_text_on_screen(*stop_data,
                            1,10,ViewBase::JUSTIFY_CENTER);
    }
                         

}