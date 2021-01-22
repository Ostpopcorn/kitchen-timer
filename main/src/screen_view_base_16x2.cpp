#include "screen_view.h"
#include "string.h"

//Initialization of static members
LiquidCrystal* ViewBase16x2::lcd{NULL};
char ViewBase16x2::current_screen[33];


ViewBase16x2::ViewBase16x2()
{

}


ViewBase16x2::~ViewBase16x2()
{

}

void ViewBase16x2::init()
{
    memset(ViewBase16x2::current_screen,' ',sizeof(current_screen));
    ViewBase16x2::current_screen[32] = '\0';
}

void ViewBase16x2::assignLcd(LiquidCrystal* new_lcd)
{
    lcd = new_lcd;
    lcd = lcd;
    lcd->begin(numCols, numRows);
}

void ViewBase16x2::write_text_on_screen(const std::string & to_print, uint8_t row, uint8_t col){
    
}
