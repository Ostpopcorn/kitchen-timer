#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include <stdbool.h>

#include "rotary_encoder.h"
#include "button.h"
#include "LiquidCrystal.h"

#define ROT_ENC_A_GPIO GPIO_NUM_32
#define ROT_ENC_B_GPIO GPIO_NUM_33
#define ROT_ENC_BTN_GPIO GPIO_NUM_27
#define TAG "MAIN"

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Program start...");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Create the queues for handling events
    QueueHandle_t button_event_queue = button_create_queue();
    QueueHandle_t rotary_encoder_event_queue = rotary_encoder_create_queue();
    
    // Rotary encoder init and set to half steps.
    rotary_encoder_info_t rotary_encoder_info = GENERATE_ROTARY_ENCODER_INFO_T;
    rotary_encoder_init(&rotary_encoder_info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO);
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&rotary_encoder_info, true));  
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&rotary_encoder_info, rotary_encoder_event_queue));


    // Button init 
    ESP_LOGI(TAG, "Start button");
    button_info_t button_info = GENERATE_BUTTON_INFO_T;
    gpio_num_t gpio_arr[3] = {GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_NC};
    ESP_ERROR_CHECK(button_init(&button_info, gpio_arr));
    ESP_ERROR_CHECK(button_set_queue(&button_info, button_event_queue));


    const int numRows = 2;
    const int numCols = 16;
    // LCD init 
    gpio_num_t lcd_rs = GPIO_NUM_23;
    gpio_num_t lcd_rw = GPIO_NUM_22;
    gpio_num_t lcd_en = GPIO_NUM_21;
    gpio_num_t lcd_d4 = GPIO_NUM_19;
    gpio_num_t lcd_d5 = GPIO_NUM_18;
    gpio_num_t lcd_d6 = GPIO_NUM_5;
    gpio_num_t lcd_d7 = GPIO_NUM_17;

    
    LiquidCrystal lcd(lcd_rs,lcd_rw,lcd_en,lcd_d4,lcd_d5,lcd_d6,lcd_d7);  
    lcd.begin(numCols, numRows);
    lcd.setCursor(0, 1);
    lcd.write('1');
    lcd.setCursor(3, 1);
    lcd.write('1');
    while (1)
    {
        button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
        rotary_encoder_event_t rot_ev = GENERAT_ROTARY_ENCODER_EVENT_T;
        while (xQueueReceive(button_event_queue, &(btn_ev),1) == pdPASS){
            if (btn_ev.pin == GPIO_NUM_27)
            {
                lcd.setCursor(1, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "27 rising edge");
                    lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "27 short press falling edge");
                    lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "27 long press");
                    lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "27 long press falling edge");
                    lcd.write('X');
                }
            }
            else if ((btn_ev.pin == GPIO_NUM_26) )
            {
                lcd.setCursor(4, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "26 rising edge");
                    lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "26 short press falling edge");
                    lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "26 long press");
                    lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "26 long press falling edge");
                    lcd.write('X');
                }
            }
        }
        //xQueueReceive(button_event_queue, ev,0);
        while (xQueueReceive(rotary_encoder_event_queue, &rot_ev,1) == pdPASS){
            ESP_LOGI(TAG,"%i",rot_ev.state.position);
            int number;
            lcd.setCursor(0, 0);
            if (rot_ev.state.position<0){
                lcd.write('-');
                number = -rot_ev.state.position;
            }else{
                lcd.write(' ');
                number = rot_ev.state.position;
            }

            for (size_t i = 0; i < 4; i++)
            {   
                
                lcd.setCursor(5-i, 0);
                if (number <= 0)
                {
                    lcd.write(' ');
                    lcd.setCursor(3-i, 0);
                    lcd.write(' ');
                    break;
                }
                lcd.write(number % 10 +'0');
                number /= 10;
            }
            
        } 
        
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}