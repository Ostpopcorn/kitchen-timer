#include "sdkconfig.h"
#include "screen_controller.h"
#include "screen_view_16x2.h"
#include "esp_timer.h"
#include "animator.h"


ScreenController::ScreenController()
{
    model= new ScreenModel{};
    ViewBase16x2::assing_model(model);
    model->insert(ENTRY_DUMMY_INT, 10);
    Animator::set_clock_func(esp_timer_get_time);
    model->insert( ENTRY_YES,"Yes");
    model->insert( ENTRY_NO,"No");
    model->insert( ENTRY_START,"Start");
    model->insert( ENTRY_STOP,"Stop");
    model->insert( ENTRY_PAUSE,"Pause");
    model->insert( ENTRY_CONTINUE,"Cont");
    model->insert( ENTRY_RESET,"Rst");
    model->insert( ENTRY_STARTUP,CONFIG_SCREEN_STARTUP_MESSAGE);
    model->insert( ENTRY_BATTERY_VOLTAGE,"-.--");
}

ScreenController::~ScreenController()
{
}

void ScreenController::update(bool redraw){
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
    model->insert(ENTRY_PRIMARY_TIMER,
          timer->get_primary_timer()->get_remainder_as_clock());
}
void ScreenController::handle_event_battery(Battery* battery)
{
    /*
    char text[5];
    memcpy(text,battery->get_last_mesurement_as_string().c_str(),5);
    int val = battery->get_last_mesurement();
    text[0] = '0'+(val/1000)%10;
    text[1] = ':';
    text[2] = '0'+(val/100)%10;
    text[3] = '0'+(val/10)%10;
    text[4] = '\0';
    */
    
    model->insert(ENTRY_BATTERY_VOLTAGE,battery->get_last_mesurement_display());

}

void ScreenController::set_button_info(button_info_t* new_info){
    button_info = new_info;
}

void ScreenController::update_button()
{
    if(!button_info){
        return;
    }
    if(!button_info->queue){
        return;
    }
    button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
    while (xQueueReceive((*button_info).queue, &(btn_ev),1) == pdPASS){
        if (btn_ev.pin == button_info->pin_list[0])
        {
            callback_button_1(btn_ev);
        }
        else if (btn_ev.pin == button_info->pin_list[1])
        {
            callback_button_2(btn_ev);
        }
        else if (btn_ev.pin == button_info->pin_list[2])
        {
            callback_button_3(btn_ev);
        }
        else if (btn_ev.pin == button_info->pin_list[3])
        {
            callback_button_4(btn_ev);
        }
    }
}
void ScreenController::callback_button_1(button_event_t button_event){
    if (button_event.event == BUTTON_RISING_EDGE){
        change_view(ScreenController::CLOCK_TIMER_RUNNING);
    }

}
void ScreenController::callback_button_2(button_event_t button_event){

}
void ScreenController::callback_button_3(button_event_t button_event){

}
void ScreenController::callback_button_4(button_event_t button_event){
    if (button_event.event == BUTTON_RISING_EDGE){
        change_view(ScreenController::CLOCK_TIMER_STOP);
    }
}
void ScreenController::callback_rot_changed(int){

}