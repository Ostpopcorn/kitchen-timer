#ifndef TIMER_SCREEN_VIEW_H
#define TIMER_SCREEN_VIEW_H

#include "LiquidCrystalGPIO.h"
#include "screen_model.h"
#include "screen_backlight.h"


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

public:
    View16x2Start();
    virtual ~View16x2Start();
    virtual void update() override;
};


class View16x2SimpleClock : public ViewBase16x2
{
protected:

public:
    View16x2SimpleClock();
    virtual ~View16x2SimpleClock();
    virtual void update() override;
};



#endif