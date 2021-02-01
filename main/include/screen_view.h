#ifndef TIMER_SCREEN_VIEW_H
#define TIMER_SCREEN_VIEW_H

#include "screen_model.h"


class ViewBase
{
protected:
    typedef enum {
        JUSTIFY_NONE = 0,
        JUSTIFY_LEFT = 5,
        JUSTIFY_CENTER,
        JUSTIFY_RIGHT
    } justify_t;
public:
    static ScreenModel* model;
    static void assing_model(ScreenModel* new_model);
    bool has_model();
    ViewBase();
    virtual ~ViewBase();
    virtual void update();
};




#endif