#include "sdkconfig.h"
#include "screen_controller.h"
#include "screen_view_16x2.h"
#include "esp_timer.h"
#include "animator.h"
#include <string>
#include "screen_definitions.h"

ScreenController::ScreenController()
{
    model= new DataModelType{};
    ViewBase16x2::assing_model(model);
    model->set_entry(ENTRY_DUMMY_INT, 10);
    Animator::set_clock_func(esp_timer_get_time);
    model->set_entry( ENTRY_YES,std::string("Yes"));
    model->set_entry( ENTRY_NO,std::string("No"));
    model->set_entry( ENTRY_START,std::string("Start"));
    model->set_entry( ENTRY_STOP,std::string("Stop"));
    model->set_entry( ENTRY_PAUSE,std::string("Pause"));
    model->set_entry( ENTRY_CONTINUE,std::string("Cont"));
    model->set_entry( ENTRY_RESET,std::string("Rst"));
    model->set_entry( ENTRY_STARTUP,std::string(CONFIG_SCREEN_STARTUP_MESSAGE));
    model->set_entry( ENTRY_BATTERY_VOLTAGE,std::string("-.--"));
}

ScreenController::~ScreenController()
{
}

void ScreenController::update(bool redraw){

    // First read from rotary encoder and taking action
    RotaryEncoderController* rot_enc_ctrl{dynamic_cast<RotaryEncoderController*>(current_view->get_rotary_encoder_controller())};
    if(rot_enc_ctrl){
        rot_enc_ctrl->update_rotary_encoder();
        auto& queue = rot_enc_ctrl->get_rotary_encoder_queue();
        while (!queue.empty())
        {
            auto& a = queue.front();
            (*a.first)(a.second);
            queue.pop();
        }
    }
    // This should collect all actions from the buttons and store them on a queue
    // Then all actions should be executed
    ButtonsController* btn_ctrl{dynamic_cast<ButtonsController*>(current_view->get_button_controller())};
    if (btn_ctrl){
        btn_ctrl->update_button();
        // Current view has a ButtonsController, nice
        auto& queue = btn_ctrl->get_buttons_queue();
        while (!queue.empty())
        {
            auto& a = queue.front();
            (*a.first)(a.second);
            queue.pop();
        }
        
        
    }

    // Last update the screen to show any changes.
    current_view->update(redraw);
}
void ScreenController::change_view(screen_views_t new_view){
    switch (new_view)
    {
    case CLOCK_BLANK:
        // Dafaq
        break;
    case CLOCK_WELCOME:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2Start{};
        break;
    case CLOCK_TIMER_STOP:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2ClockStop{};
        break;
    case CLOCK_TIMER_RUNNING:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2ClockRunning{};
        break;
    case CLOCK_TIMER_PAUSE:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2ClockPause{};
        break;
    case CLOCK_TIMER_ALARM:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2ClockAlarm{};
        break;
    default:
        break;
    }
    update(true);
}
void ScreenController::handle_event_timer(TimerContainer* timer)
{
    model->set_entry(ENTRY_PRIMARY_TIMER,
          timer->get_primary_timer()->get_remainder_as_clock());
}
void ScreenController::handle_event_battery(Battery* battery)
{
    model->set_entry(ENTRY_BATTERY_VOLTAGE,battery->get_last_mesurement_display());
}

