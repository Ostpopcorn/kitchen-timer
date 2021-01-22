#include "screen_view.h"
#include "string.h"

ViewBase16x2::ViewBase16x2()
{

}

ViewBase16x2::ViewBase16x2(ScreenModel & model) : ViewBase(model)
{
    
}

ViewBase16x2::~ViewBase16x2()
{

}

void ViewBase16x2::init()
{
    memset(current_screen,' ',sizeof(current_screen));
}

void ViewBase16x2::assignLcd(LiquidCrystal & new_lcd)
{
    this->lcd = new_lcd;
}

