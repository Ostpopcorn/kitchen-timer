#ifndef TIMER_SCREEN_CONTROLLER_H
#define TIMER_SCREEN_CONTROLLER_H

#include "screen_view.h"
#include "timer_class.h"
#include "battery_monitor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "screen_definitions.h"

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
    DataModelType* model{NULL};
    ViewBase* current_view{NULL};
    
public:
    ScreenController();
    virtual ~ScreenController();
    void handleEvent();
    void change_view(screen_views_t new_view); 
    void update(bool redraw = false);
    void handle_event_timer(TimerContainer* timer);
    void handle_event_battery(Battery* battery);
};



#endif