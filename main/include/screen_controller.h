#ifndef TIMER_SCREEN_CONTROLLER_H
#define TIMER_SCREEN_CONTROLLER_H

#include "screen_model.h"
#include "screen_view.h"
#include "timer_class.h"

class ScreenController
{
public:
    typedef enum {
        CLOCK_BLANK = 0,
        CLOCK_WELCOME = 1,
        CLOCK_SHOW_TIMER = 10,
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
    void handle_event_timer(Timer* timer);
};



#endif