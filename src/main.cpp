#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"


#include <stdbool.h>

#include "rotary_encoder.h"
#include "button.h"
#include "Sound.h"
#include "Screen.h"
#include "LiquidCrystalGPIO.h"


#define TAG "MAIN"

extern "C" void app_main()
{
    ESP_LOGI(TAG, "Program start...");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    
    gpio_num_t enable_5V_pin = GPIO_NUM_23;
    gpio_num_t battery_monitor_enable_pin = GPIO_NUM_32;
    gpio_num_t battery_monitor_analog_pin = GPIO_NUM_33;
    gpio_num_t ws2812b_data_pin = GPIO_NUM_2;
    gpio_num_t lcd_dimmer = GPIO_NUM_4;
    uint64_t pin_mask_ctrl = (1ULL<<enable_5V_pin) | 
                             (1ULL<<battery_monitor_enable_pin)| 
                             (1ULL<<ws2812b_data_pin)| 
                             (1ULL<<lcd_dimmer);

    gpio_config_t io_conf = {
        .pin_bit_mask = pin_mask_ctrl,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(enable_5V_pin, 1);
    gpio_set_level(lcd_dimmer, 1);

    // Create the queues for handling events
    QueueHandle_t button_event_queue = button_create_queue();
    QueueHandle_t rotary_encoder_event_queue = rotary_encoder_create_queue();
    
    gpio_num_t rot_enc_a_gpio = GPIO_NUM_39;
    gpio_num_t rot_enc_b_gpio = GPIO_NUM_34;
    gpio_num_t rot_enc_btn_gpio = GPIO_NUM_35;

    // Rotary encoder init and set to half steps.
    rotary_encoder_info_t rotary_encoder_info = GENERATE_ROTARY_ENCODER_INFO_T;
    rotary_encoder_init(&rotary_encoder_info, rot_enc_b_gpio, rot_enc_a_gpio);
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&rotary_encoder_info, true));  
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&rotary_encoder_info, rotary_encoder_event_queue));

    // Sound sound{};    // Need to init this before the buttons in order to have btn_2 work since it uses DAC_2 pin
    Screen screen{};  // 
    screen.set_backlight_gpio(lcd_dimmer);
    screen.fade_backlight_to(0xff);
    // Button init 
    gpio_num_t btn_1 = GPIO_NUM_27;
    gpio_num_t btn_2 = GPIO_NUM_26;
    gpio_num_t btn_3 = GPIO_NUM_36;

    ESP_LOGI(TAG, "Start button");
    button_info_t button_info = GENERATE_BUTTON_INFO_T;
    gpio_num_t btn_gpio_arr[5] = {btn_1, btn_2,btn_3,rot_enc_btn_gpio, GPIO_NUM_NC};
    ESP_ERROR_CHECK(button_init(&button_info, btn_gpio_arr));
    ESP_ERROR_CHECK(button_set_queue(&button_info, button_event_queue));


    ESP_LOGI(TAG, "Starting screen");
    const int numRows = 2;
    const int numCols = 16;
    // LCD init 
    gpio_num_t lcd_rs = GPIO_NUM_21;
    gpio_num_t lcd_rw = GPIO_NUM_NC;
    gpio_num_t lcd_en = GPIO_NUM_19;

    gpio_num_t lcd_d4 = GPIO_NUM_18;
    gpio_num_t lcd_d5 = GPIO_NUM_5;
    gpio_num_t lcd_d6 = GPIO_NUM_17;
    gpio_num_t lcd_d7 = GPIO_NUM_16;

            
    LiquidCrystalGPIO lcd(LiquidCrystal::bit_mode::FOUR_BIT 
                    ,lcd_rs,lcd_rw,lcd_en,
                    lcd_d4,lcd_d5,lcd_d6,lcd_d7,
                    GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC);
    lcd.begin(numCols, numRows);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd.setCursor(0, 0);
    lcd.write('H');
    lcd.setCursor(1,0);
    lcd.write('E');
    lcd.setCursor(2, 0);
    lcd.write('J');
    ESP_LOGI(TAG,"Setup done!");


    while (1)
    {
        button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
        rotary_encoder_event_t rot_ev = GENERAT_ROTARY_ENCODER_EVENT_T;
        while (xQueueReceive(button_event_queue, &(btn_ev),1) == pdPASS){
            if (btn_ev.pin == btn_1)
            {
                lcd.setCursor(1, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_1 rising edge");
                    lcd.write('R');
                    screen.fade_backlight_to(0xff/2);
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_1 short press falling edge");
                    lcd.write('F');
                    screen.fade_backlight_to(0xff);
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_1 long press");
                    lcd.write('L');
                    screen.fade_backlight_to(0xff/4);
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_1 long press falling edge");
                    lcd.write('X');
                    screen.fade_backlight_to(0xff);
                }
            }
            else if ((btn_ev.pin == btn_2) )
            {
                lcd.setCursor(2, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_2 rising edge");
                    lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_2 short press falling edge");
                    lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_2 long press");
                    lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_2 long press falling edge");
                    lcd.write('X');
                }
            }
            else if ((btn_ev.pin == btn_3) )
            {
                lcd.setCursor(3, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_3 rising edge");
                    lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_3 short press falling edge");
                    lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_3 long press");
                    lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_3 long press falling edge");
                    lcd.write('X');
                }
            }
            else if ((btn_ev.pin == rot_enc_btn_gpio) )
            {
                lcd.setCursor(4, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio rising edge");
                    lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio short press falling edge");
                    lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio long press");
                    lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio long press falling edge");
                    lcd.write('X');
                }
            }
        }
        //xQueueReceive(button_event_queue, ev,0);
        while (xQueueReceive(rotary_encoder_event_queue, &rot_ev,1) == pdPASS){
            //ESP_LOGI(TAG,"%i",rot_ev.state.position);
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
        
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
