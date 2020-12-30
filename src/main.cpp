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
#include "Timer.h"
#include "LiquidCrystalGPIO.h"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

#define TAG "MAIN"
static void inline print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\n", (uint32_t) (counter_value >> 32),
           (uint32_t) (counter_value));
    printf("Time   : %.8f s\n", (double) counter_value / TIMER_SCALE);
}

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

    //Sound sound{};    // Need to init this before the buttons in order to have btn_2 work since it uses DAC_2 pin


    ESP_LOGI(TAG, "Starting screen");
    // LCD init 
    gpio_num_t lcd_rs = GPIO_NUM_21;
    gpio_num_t lcd_rw = GPIO_NUM_NC;
    gpio_num_t lcd_en = GPIO_NUM_19;

    gpio_num_t lcd_d4 = GPIO_NUM_18;
    gpio_num_t lcd_d5 = GPIO_NUM_5;
    gpio_num_t lcd_d6 = GPIO_NUM_17;
    gpio_num_t lcd_d7 = GPIO_NUM_16;

            
    LiquidCrystalGPIO* lcd = new LiquidCrystalGPIO(LiquidCrystal::bit_mode::FOUR_BIT 
                    ,lcd_rs,lcd_rw,lcd_en,
                    lcd_d4,lcd_d5,lcd_d6,lcd_d7,
                    GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC);
    // lcd.begin(numCols, numRows);
    Screen<LiquidCrystalGPIO*> screen{lcd,lcd_dimmer};  // 
    // screen.set_backlight_gpio(lcd_dimmer);
    screen.fade_backlight_to(0xff);

    Timer timer;
    timer.set_alarm_value(122);

    // Button init 
    gpio_num_t btn_1 = GPIO_NUM_27;
    gpio_num_t btn_2 = GPIO_NUM_26;
    gpio_num_t btn_3 = GPIO_NUM_36;

    ESP_LOGI(TAG, "Start button");
    button_info_t button_info = GENERATE_BUTTON_INFO_T;
    gpio_num_t btn_gpio_arr[5] = {btn_1, btn_2,btn_3,rot_enc_btn_gpio, GPIO_NUM_NC};
    ESP_ERROR_CHECK(button_init(&button_info, btn_gpio_arr));
    ESP_ERROR_CHECK(button_set_queue(&button_info, button_event_queue));

    button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
    rotary_encoder_event_t rot_ev = GENERAT_ROTARY_ENCODER_EVENT_T;
    timer_event_t timer_event = GENERAT_TIMER_EVENT_T;
    // xQueueHandle timer_queue = timer.get_queue_handle();

    int32_t mem_len = 16;
    int32_t prev_rot_enc_pos[mem_len];
    uint64_t prev_rot_enc_time[mem_len];
    prev_rot_enc_time[0] = 0;

    ESP_LOGI(TAG,"Setup done!");

    while (1)
    {
        while (xQueueReceive(button_event_queue, &(btn_ev),1) == pdPASS){
            if (btn_ev.pin == btn_1)
            {
                // lcd.setCursor(1, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_1 rising edge");
                    // lcd.write('R');
                    screen.fade_backlight_to(0xff/2);
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_1 short press falling edge");
                    // lcd.write('F');
                    screen.fade_backlight_to(0xff);
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_1 long press");
                    // lcd.write('L');
                    screen.fade_backlight_to(0xff/4);
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_1 long press falling edge");
                    // lcd.write('X');
                    screen.fade_backlight_to(0xff);
                }
            }
            else if ((btn_ev.pin == btn_2) )
            {
                // lcd.setCursor(2, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_2 rising edge");
                    timer.start();
                    // lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_2 short press falling edge");
                    // lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_2 long press");
                    // lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_2 long press falling edge");
                    // lcd.write('X');
                }
            }
            else if ((btn_ev.pin == btn_3) )
            {
                // lcd.setCursor(3, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_3 rising edge");
                    
                    timer.pause();
                    // lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_3 short press falling edge");
                    // lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_3 long press");
                    // lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_3 long press falling edge");
                    // lcd.write('X');
                }
            }
            else if ((btn_ev.pin == rot_enc_btn_gpio) )
            {
                // lcd.setCursor(4, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio rising edge");
                    // lcd.write('R');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio short press falling edge");
                    // lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio long press");
                    // lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "rot_enc_btn_gpio long press falling edge");
                    // lcd.write('X');
                }
            }
        }
        //xQueueReceive(button_event_queue, ev,0);
        while (xQueueReceive(rotary_encoder_event_queue, &rot_ev,1) == pdPASS){
            //ESP_LOGI(TAG,"%i",rot_ev.state.position);
            // Make sure there is more than 10 ms between the different events to make 
            // have deacent precision

            for (size_t i = 0; i < mem_len-1; i++)
            {
                prev_rot_enc_pos[mem_len-1-i] = prev_rot_enc_pos[mem_len-1-i-1];
                prev_rot_enc_time[mem_len-1-i]  = prev_rot_enc_time[mem_len-1-i-1]; 
            }

            int64_t curr_micros{esp_timer_get_time()};
            // When there has not been an update in a while we might as well reset the counter of the rotary encoder
            // to make sure it never leaves its bounds.
            int64_t rot_enc_reset_time{10000000};
            // How long it will keep history
            int64_t history_time_length{500000};

            // Rot enc reset condition, too long has passed between the last two events.
            if ((curr_micros-prev_rot_enc_time[0])>(rot_enc_reset_time)){
                printf("rot reset,%llu",(curr_micros-prev_rot_enc_time[0]));
                rotary_encoder_reset(&rotary_encoder_info);
                prev_rot_enc_pos[0]   = 0;
            }else{

                prev_rot_enc_pos[0]   = rot_ev.state.position;
            }
            prev_rot_enc_time[0]  = curr_micros;
            
            
            int len_of_vec_to_use{0};
            for (size_t i = 0; i < mem_len-1; i++)
            {
                // Set one to zero for samples older than 1 sec
                if ( (prev_rot_enc_time[0] - prev_rot_enc_time[i]) > history_time_length ){
                    // prev_rot_enc_time[i] = 0;
                    len_of_vec_to_use = i;
                    printf("con len is: %i ",i);
                    break;
                }
                
            }
            
            
            // Now the previous vector is updated and it can be evaluated
            double conv{0};
            int rot_enc_change{prev_rot_enc_pos[0]-prev_rot_enc_pos[len_of_vec_to_use-1]};
            int rot_enc_change_abs{rot_enc_change*( (int) ((rot_enc_change>=0)?1:-1) )};
            // ESP_LOGI("a","%i %i %i",prev_rot_enc_pos[0],prev_rot_enc_pos[1],prev_rot_enc_pos[2]);
            if (len_of_vec_to_use == 0){
                printf("ERROR");
            } else if (len_of_vec_to_use == 1){
                if (prev_rot_enc_pos[0]>0){
                    conv = 1;
                }else{
                    conv = -1;
                }
            } else {
                // First set the amout to scroll by
                if (rot_enc_change_abs>=25){
                    conv = 15;
                } else if (rot_enc_change_abs>=15){
                    conv = 5;
                }else{
                    conv = 1;
                }
                // Then correct its sign
                conv *= ( (double) ((rot_enc_change>=0)?1:-1) );
            }
            printf("change %i, conv %f\n",rot_enc_change,conv);

            timer.change_alarm_value(conv);
        
        } 
        
        while (xQueueReceive(timer.get_queue_handle(), &timer_event,1) == pdPASS){
            /* Print information that the timer reported an event */
            if (timer_event.type == 1) {
                printf("\n    Example timer without reload\n");
            } else if (timer_event.type == 0) {
                printf("\n    Example timer with auto reload\n");
            } else {
                printf("\n    UNKNOWN EVENT TYPE\n");
            }
            printf("Group[%d], timer[%d] alarm event\n", timer_event.timer_group, timer_event.timer_idx);

            /* Print the timer values passed by event */
            printf("------- EVENT TIME --------\n");
            print_timer_counter(timer_event.timer_counter_value);

            /* Print the timer values as visible by this task */
            printf("-------- TASK TIME --------\n");
            uint64_t task_counter_value;
            timer_get_counter_value(timer_event.timer_group, timer_event.timer_idx, &task_counter_value);
            print_timer_counter(task_counter_value);
        }
        //ESP_LOGI("H","%f",timer.get_remainder_as_double(TIMER_0));
        screen.update(&timer);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
