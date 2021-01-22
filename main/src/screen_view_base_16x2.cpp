#include "screen_view.h"
#include "string.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define TAG "Base16x2"

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
    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd->clear();
}

void ViewBase16x2::write_text_on_screen(const std::string & to_print, int row, int col, justify_t alignment)
{
    int print_len = to_print.length();
    if(print_len <= 0){
        return;
        }
    // int print_len = to_print.length()>16?16:to_print.length();

    // Check how well it fits inside the bounds of the display
    // Then sets the starting position to start printing in
    // Checking out bound 
    if ( 0> row || row >= numRows ||  0 > col || col >= numCols){
        return; // ERROR
    }
    if ( alignment == JUSTIFY_LEFT){
        if ( (col+print_len-1) >= numCols){
            return; // ERROR
        }
        // col = col
    } else if ( alignment == JUSTIFY_CENTER){
        int extent = print_len/2;
        if( ( 0>(col-extent)) || ((col+extent)>= numCols) ){
            return; // ERROR
        }
        col -= extent;
    }else if ( alignment == JUSTIFY_RIGHT){
        if ( 0 > (col-print_len+1) ){
            return; // ERROR
        }
        col = col-print_len+1;
    }else{
        //ERROR
    }

    // Compare to what is currently on screen
    // return if the content match
    if (memcmp(to_print.c_str(),current_screen,print_len)==0){
        ESP_LOGI(TAG,"O %s.",current_screen);

        return; // message is allready on screen
    }

    //print out on the screen
    for (size_t i = 0; i < (print_len); i++)
    {
        lcd->setCursor(i+col, row);
        lcd->write(to_print[i]);
        current_screen[i+col] = to_print[i]; 
    }
    ESP_LOGI(TAG,"U %s.",current_screen);
}
