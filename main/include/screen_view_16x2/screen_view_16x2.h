#ifndef TIMER_SCREEN_VIEW_16x2_H
#define TIMER_SCREEN_VIEW_16x2_H

#include "screen_view.h"
#include "LiquidCrystalGPIO.h"
#include "screen_backlight.h"
#include "animator.h"




class ViewBase16x2 : public ViewBase
{
private:
    void init();
    static const int numRows{2};
    static const int numCols{16};
protected:
    static LiquidCrystal* lcd;
    static LcdBacklight* backlight;
    static char current_screen[];
    void write_text_on_screen(const std::string & to_print, int row, int col, justify_t alignment);

public:
    ViewBase16x2();
    virtual ~ViewBase16x2();

    static void assignLcd(LiquidCrystal *);
    static void assignBacklight(LcdBacklight *);
    bool has_lcd();
    bool has_backlight();
};



class View16x2Start : public ViewBase16x2
{
protected:
    AnimationSpinningWheel anim{};
public:
    View16x2Start();
    virtual ~View16x2Start();
    virtual void update(bool redraw = true) override;

    static ButtonsController* button_controller;
    ButtonsControllerBase* get_button_controller() override {return button_controller;};
};


class View16x2ClockStop : public ViewBase16x2
{
protected:

public:
    View16x2ClockStop();
    virtual ~View16x2ClockStop();
    virtual void update(bool redraw = true) override;
    
    static RotaryEncoderController* rotary_encoder_controller;
    RotaryEncoderControllerBase* get_rotary_encoder_controller() override {return rotary_encoder_controller;};
};


class View16x2ClockPause : public ViewBase16x2
{
protected:

public:
    View16x2ClockPause();
    virtual ~View16x2ClockPause();
    virtual void update(bool redraw = true) override;
};


class View16x2ClockRunning : public ViewBase16x2
{
protected:

public:
    View16x2ClockRunning();
    virtual ~View16x2ClockRunning();
    virtual void update(bool redraw = true) override;
};

class View16x2ClockAlarm : public ViewBase16x2
{
protected:
public:
    View16x2ClockAlarm();
    virtual ~View16x2ClockAlarm();
    virtual void update(bool redraw = true) override;
};



#endif