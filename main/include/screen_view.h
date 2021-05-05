#ifndef TIMER_SCREEN_VIEW_H
#define TIMER_SCREEN_VIEW_H

#include "screen_definitions.h"
#include "buttons_controller.h"
#include "roatry_encoder_controller.h"

class ViewBase
{
protected:
    typedef enum {
        JUSTIFY_NONE = 0,
        JUSTIFY_LEFT = 5,
        JUSTIFY_CENTER,
        JUSTIFY_RIGHT
    } justify_t;
    static DataModelType* model;
public:
    virtual ButtonsControllerBase* get_button_controller(){return nullptr;};
    virtual RotaryEncoderControllerBase* get_rotary_encoder_controller(){return nullptr;};
    static void assing_model(DataModelType* new_model);
    bool has_model();
    ViewBase();
    virtual ~ViewBase();
    virtual void update(bool redraw = true);
};




#endif