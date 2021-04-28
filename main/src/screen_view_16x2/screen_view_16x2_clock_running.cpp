#include "screen_view_16x2.h"
#include "string.h"
#include "esp_log.h"
#include "screen_model_entry_types.h"
#include "counter_clock.h"

#define TAG "SVIEW"


ButtonsController* View16x2ClockRunning::button_controller{new ButtonsController{}};

View16x2ClockRunning::View16x2ClockRunning()
{
    if(has_backlight()){
        backlight->fade_to(255);
    }
}

View16x2ClockRunning::~View16x2ClockRunning()
{
}

void View16x2ClockRunning::update(bool redraw)
{
    anim.update();
    write_text_on_screen("R",
                         0,0,ViewBase::JUSTIFY_CENTER);
    // Dela upp i "kör" och "står still" och "alarm"
    auto prmy_clk = model->get_entry<CounterClock>(ENTRY_PRIMARY_TIMER);



    if (redraw || prmy_clk.value_has_been_updated() || anim.value_has_been_updated()) {
        write_text_on_screen(prmy_clk.get_object().to_string(' ',anim.get_char()),
                            0,7,ViewBase::JUSTIFY_CENTER);
    }
    write_text_on_screen(model->get_entry_object<std::string>(ENTRY_STOP),
                         1,11,ViewBase::JUSTIFY_CENTER);
                         

}