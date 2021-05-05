#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include <stdbool.h>
#include <limits>

#include "rotary_encoder.h"
#include "button.h"
#include "Sound.h"
#include "screen_controller.h"
#include "screen_view_16x2/screen_view_16x2.h"
#include "timer_class.h"
#include "LiquidCrystalGPIO.h"
#include "buttons_controller.h"
#include "roatry_encoder_controller.h"
#include "battery_monitor.h"

#define TAG "MAIN"

extern "C" void app_main()
{
    #define STRING2(x) #x
    #define STRING(x) STRING2(x)
    #pragma message(STRING(__cplusplus))
    // esp_log_set_vprintf(esp_apptrace_vprintf);
    
    ESP_LOGI(TAG, "Soon...");
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "Program start...");
    ESP_LOGI(TAG, "Int lim: %i", std::numeric_limits<int>::max());
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    

    gpio_num_t enable_5V_pin = GPIO_NUM_23;
    
    // Need both ADC channel and GPIO NUM
    gpio_num_t battery_monitor_enable_pin = GPIO_NUM_32;

    // gpio_num_t battery_monitor_analog_pin = GPIO_NUM_33; 
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

    // Always start with 5V enabeld to ensure screen is on.
    gpio_set_level(enable_5V_pin, 1);
    
    // Create the queues for handling events
    QueueHandle_t button_event_queue = button_create_queue();
    QueueHandle_t rotary_encoder_event_queue = rotary_encoder_create_queue();

     // Button & rotary encoder setup init 
    gpio_num_t btn_1 = GPIO_NUM_27;
    gpio_num_t btn_2 = GPIO_NUM_26;
    gpio_num_t btn_3 = GPIO_NUM_36;
    gpio_num_t rot_enc_btn_gpio = GPIO_NUM_35;
    gpio_num_t rot_enc_a_gpio = GPIO_NUM_39;
    gpio_num_t rot_enc_b_gpio = GPIO_NUM_34;

    button_info_t button_info = GENERATE_BUTTON_INFO_T;
    gpio_num_t btn_gpio_arr[5] = {btn_1, btn_2,btn_3,rot_enc_btn_gpio, GPIO_NUM_NC};
    ESP_ERROR_CHECK(button_init(&button_info, btn_gpio_arr));
    ESP_ERROR_CHECK(button_set_queue(&button_info, button_event_queue));
    
    
    // Rotary encoder init and set to half steps.
    rotary_encoder_info_t rotary_encoder_info = GENERATE_ROTARY_ENCODER_INFO_T;
    rotary_encoder_init(&rotary_encoder_info, rot_enc_b_gpio, rot_enc_a_gpio);
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&rotary_encoder_info, true));  
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&rotary_encoder_info, rotary_encoder_event_queue));

    // Pass info objects to controllers
    ButtonsController::set_button_info(&button_info);
    RotaryEncoderController::set_rotary_encoder_info(&rotary_encoder_info);

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

    // Screen controller start and callbacks assignment.
    ScreenController* controller = new ScreenController{};
    controller->change_view(ScreenController::CLOCK_WELCOME);
    
    TimerContainer* timer = new TimerContainer{};
    timer->get_primary_timer()->set_alarm_value((98*3600)+122);
    
    // Welcome screen
    View16x2Start::button_controller->callback_button_1 = [](button_state_t state){
        ESP_LOGI("CB","BUTTANSNSNSNS!! %i",state);
    };
    View16x2Start::button_controller->callback_button_2 = [controller](button_state_t state){
        switch (state)
        {
        case BUTTON_RISING_EDGE:
            controller->change_view(ScreenController::CLOCK_TIMER_STOP);
            break;
        
        default:
            break;
        }
    };
    // Clock stop
    View16x2ClockStop::button_controller->callback_button_2 = [controller,timer](button_state_t state){
        switch (state)
        {
        case BUTTON_RISING_EDGE:
            timer->get_primary_timer()->start();
            controller->change_view(ScreenController::CLOCK_TIMER_RUNNING);
            break;
        
        default:
            break;
        }
    };
    View16x2ClockStop::button_controller->callback_button_4 = [controller](button_state_t state){
        switch (state)
        {
        case BUTTON_RISING_EDGE:
            controller->change_view(ScreenController::CLOCK_WELCOME);
            break;
        
        default:
            break;
        }
    };

    View16x2ClockStop::rotary_encoder_controller->callback_rot_changed = [timer](int amount){
        timer->get_primary_timer()->change_alarm_value(amount);
    };

    // Clock running
    
    View16x2ClockRunning::button_controller->callback_button_1 = [controller,timer](button_state_t state){
        switch (state)
        {
        case BUTTON_RISING_EDGE:
            timer->get_primary_timer()->pause();
            controller->change_view(ScreenController::CLOCK_TIMER_STOP);
            break;
        
        default:
            break;
        }
    };

    // Timer callbacks
    timer->register_callback((TimerContainer::timer_id_t) 1,TimerContainer::EVENT_TYPE_START,[](TimerContainer::timer_event_t const timer_event)
    {
        ESP_LOGI("CB","Callback timer start. name: %s",(*timer_event.timer_info).get_name().c_str());
    });

    timer->register_callback((TimerContainer::timer_id_t) 1,TimerContainer::EVENT_TYPE_STOP,[](TimerContainer::timer_event_t const timer_event)
    {
        ESP_LOGI("CB","Callback timer stop. name: %s",(*timer_event.timer_info).get_name().c_str());
       
    });

    // Battery callback
    battery_monitor->register_callback([controller](Battery* battery){
        controller->handle_event_battery(battery);
        ESP_LOGI("CB","BATTERY.");
    });
  
    ESP_LOGI(TAG,"Setup done!");

    battery_monitor->measure();

    while (1)
    {
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
