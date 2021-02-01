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
#include "screen_model.h"
#include "screen_controller.h"
#include "screen_view_16x2/screen_view_16x2.h"
#include "timer_class.h"
#include "LiquidCrystalGPIO.h"

#include "battery_monitor.h"

#define TAG "MAIN"

extern "C" void app_main()
{

    // esp_log_set_vprintf(esp_apptrace_vprintf);
    
    ESP_LOGI(TAG, "Soon...");
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Program start...");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    

    gpio_num_t enable_5V_pin = GPIO_NUM_23;
    
    // Need both ADC channel and GPIO NUM
    gpio_num_t battery_monitor_enable_pin = GPIO_NUM_32;

    gpio_num_t battery_monitor_analog_pin = GPIO_NUM_33; 
    adc1_channel_t battery_monitor_analog_adc1_channel = ADC1_CHANNEL_5;

    Battery* battery_monitor = new Battery{battery_monitor_analog_adc1_channel,battery_monitor_enable_pin};

    //adc1_config_width(ADC_WIDTH_BIT_12);
    //adc1_config_channel_atten(battery_monitor_analog_adc1_channel,ADC_ATTEN_DB_11);
    
    
    gpio_num_t ws2812b_data_pin = GPIO_NUM_2;
    gpio_num_t lcd_dimmer = GPIO_NUM_4;
    uint64_t pin_mask_ctrl = (1ULL<<enable_5V_pin) | 
                             //(1ULL<<battery_monitor_enable_pin)| 
                             //(1ULL<<lcd_dimmer) |
                             (1ULL<<ws2812b_data_pin) 
                             ;

    gpio_config_t io_conf = {
        .pin_bit_mask = pin_mask_ctrl,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(enable_5V_pin, 1);
    //gpio_set_level(lcd_dimmer, 1);
    // gpio_set_level(battery_monitor_enable_pin, 1);
    
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


    LiquidCrystalGPIO* lcd= new LiquidCrystalGPIO{LiquidCrystal::bit_mode::FOUR_BIT 
                    ,lcd_rs,lcd_rw,lcd_en,
                    lcd_d4,lcd_d5,lcd_d6,lcd_d7,
                    GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC};
    ViewBase16x2::assignLcd(lcd);
    LcdBacklight * backlight = new LcdBacklight{lcd_dimmer};
    backlight->set_fade_time(1000);
    backlight->fade_to(0xff);
    ViewBase16x2::assignBacklight(backlight);

    ScreenController* controller = new ScreenController{};
    controller->change_view(ScreenController::CLOCK_WELCOME);
    

    TimerContainer* timer = new TimerContainer{};
    timer->get_primary_timer()->set_alarm_value(122);
    /*
    */
    timer->register_callback((TimerContainer::timer_id_t) 1,TimerContainer::EVENT_TYPE_START,[](TimerContainer::timer_event_t const timer_event)
    {
        ESP_LOGI("CB","Callback timer start. name: %s",(*timer_event.timer_info).get_name().c_str());
    });

    timer->register_callback((TimerContainer::timer_id_t) 1,TimerContainer::EVENT_TYPE_STOP,[](TimerContainer::timer_event_t const timer_event)
    {
        ESP_LOGI("CB","Callback timer stop. name: %s",(*timer_event.timer_info).get_name().c_str());
       
    });

    battery_monitor->register_callback([controller](Battery* battery){
        controller->handle_event_battery(battery);
        ESP_LOGI("CB","BATTERY.");
    });


    // [captures list, vad den känner till utifrån, &](parametrar in till funktionen){kroppen};
    
    //battery_monitor.register_callback([&controller](Battery* battery){
    //    controller->handle_event_battery(battery);
    //});

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
    // timer_event_t timer_event = GENERAT_TIMER_EVENT_T;
    // xQueueHandle timer_queue = timer.get_queue_handle();

    int32_t mem_len = 32;
    int32_t prev_rot_enc_pos[mem_len];
    uint64_t prev_rot_enc_time[mem_len];
    prev_rot_enc_time[0] = 0;

    ESP_LOGI(TAG,"Setup done!");

    battery_monitor->measure();

    while (1)
    {
        while (xQueueReceive(button_event_queue, &(btn_ev),1) == pdPASS){
            if (btn_ev.pin == btn_1)
            {
                // lcd.setCursor(1, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_1 rising edge");
                    // lcd.write('R');
                    //screen.change_view(ScreenBase::screen_views::CLOCK_SHOW_TIMER);
                    timer->get_primary_timer()->pause();
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE){
                    ESP_LOGI(TAG, "btn_1 short press falling edge");
                    // lcd.write('F');
                }
                else if (btn_ev.event == BUTTON_LONG_PRESS){
                    ESP_LOGI(TAG, "btn_1 long press");
                    // lcd.write('L');
                }
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG){
                    ESP_LOGI(TAG, "btn_1 long press falling edge");
                    // lcd.write('X');
                }
            }
            else if ((btn_ev.pin == btn_2) )
            {
                // lcd.setCursor(2, 1);
                if (btn_ev.event == BUTTON_RISING_EDGE){
                    ESP_LOGI(TAG, "btn_2 rising edge");
                    timer->get_primary_timer()->start();
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
                    controller->change_view(ScreenController::CLOCK_SHOW_TIMER);
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

            timer->get_primary_timer()->change_alarm_value(conv);
        
        } 
        /*
        while (xQueueReceive(timer.get_queue_handle(), &timer_event,1) == pdPASS){
            // Print information that the timer reported an event 
            if (timer_event.type == 1) {
                printf("\n    Example timer without reload\n");
            } else if (timer_event.type == 0) {
                printf("\n    Example timer with auto reload\n");
            } else {
                printf("\n    UNKNOWN EVENT TYPE\n");
            }
            printf("Group[%d], timer[%d] alarm event\n", timer_event.timer_group, timer_event.timer_idx);

            // Print the timer values passed by event 
            //printf("------- EVENT TIME --------\n");
            //print_timer_counter(timer_event.timer_counter_value);

            // Print the timer values as visible by this task 
            //printf("-------- TASK TIME --------\n");
            //uint64_t task_counter_value;
            //timer_get_counter_value(timer_event.timer_group, timer_event.timer_idx, &task_counter_value);
            //print_timer_counter(task_counter_value);
        }
        */
        //ESP_LOGI("H","%f",timer.get_remainder_as_double(TIMER_0));
        //screen.update(&timer);
        controller->handle_event_timer(timer);
        controller->update();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
