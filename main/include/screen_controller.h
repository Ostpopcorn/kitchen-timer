#ifndef TIMER_SCREEN_CONTROLLER_H
#define TIMER_SCREEN_CONTROLLER_H

#include "screen_model.h"
#include "screen_view.h"
#include "timer_class.h"
#include "battery_monitor.h"
#include "screen_model_entry_types.h"
#include "button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class ScreenController
{
public:
    typedef enum {
        CLOCK_BLANK = 0,
        CLOCK_WELCOME = 1,
        CLOCK_TIMER_STOP = 10,
        CLOCK_TIMER_RUNNING,
        CLOCK_TIMER_ALARM,
        CLOCK_TIMER_PAUSE,
    } screen_views_t;
private:
    ScreenModel* model{NULL};
    ViewBase* current_view{NULL};
    
public:
    ScreenController();
    virtual ~ScreenController();
    void handleEvent();
    void change_view(screen_views_t new_view);
    void update();
    void handle_event_timer(TimerContainer* timer);
    void handle_event_battery(Battery* battery);
    void set_button_info(button_info_t* new_info);
    void update_button();
private:
    void callback_button_1(button_event_t);
    void callback_button_2(button_event_t);
    void callback_button_3(button_event_t);
    void callback_button_4(button_event_t);
    void callback_rot_changed(int);

    button_info_t* button_info;
};



#endif