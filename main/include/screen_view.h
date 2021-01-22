#ifndef TIMER_SCREEN_VIEW_H
#define TIMER_SCREEN_VIEW_H

#include "LiquidCrystalGPIO.h"
#include "screen_model.h"


class ViewBase
{
protected:
    ScreenModel model{};
public:
    ViewBase();
    ViewBase(ScreenModel & model);
    virtual ~ViewBase();
    virtual void update();
};



class ViewBase16x2 : public ViewBase
{
private:
    void init();
protected:
    static LiquidCrystal lcd;
    static char current_screen[32];
    
public:
    ViewBase16x2();
    ViewBase16x2(ScreenModel & model);
    virtual ~ViewBase16x2();

    void assignLcd(LiquidCrystal &);
    
    void set_backlight_gpio(gpio_num_t gpio);
};



class View16x2Start : public ViewBase16x2
{
protected:

public:
    View16x2Start();
    View16x2Start(ScreenModel & model);
    virtual ~View16x2Start();
    virtual void update() override;
};



#endif