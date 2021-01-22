#include "screen_view.h"
#include "string.h"
#include "esp_log.h"
#define TAG "SVIEW"
View16x2Start::View16x2Start()
{
}


View16x2Start::~View16x2Start()
{
}

void View16x2Start::update()
{
    //
    std::string message = model.get_entry_string(ScreenModelEntry::ENTRY_STARTUP);
    int print_len = message.length()>16?16:message.length();
    if (memcmp(message.c_str(),current_screen,print_len)==0){
        ESP_LOGI(TAG,"O %s",current_screen);
        return; // message is allready on screen
    }
    ESP_LOGI(TAG,"Setting welcome message");
    //vTaskDelay(50 / portTICK_PERIOD_MS);
    for (size_t i = 0; i < print_len; i++)
    {
        lcd->setCursor(i, 0);
        lcd->write(message[i]);
        current_screen[i] = message[i]; 
    }
    ESP_LOGI(TAG,"U %s",current_screen);
}