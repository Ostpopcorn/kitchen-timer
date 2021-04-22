#ifndef TIMER_SCREEN_VIEW_H
#define TIMER_SCREEN_VIEW_H

#include "screen_model.h"
#include "screen_model_entry_types.h"
#include "buttons_controller.h"

class ViewBase
{
protected:
    typedef enum {
        JUSTIFY_NONE = 0,
        JUSTIFY_LEFT = 5,
        JUSTIFY_CENTER,
        JUSTIFY_RIGHT
    } justify_t;
    static ScreenModel* model;
public:
    virtual ButtonsControllerBase* get_button_controller(){return nullptr;};
    static void assing_model(ScreenModel* new_model);
    bool has_model();
    ViewBase();
    virtual ~ViewBase();
    virtual void update(bool redraw = true);
};




#endif