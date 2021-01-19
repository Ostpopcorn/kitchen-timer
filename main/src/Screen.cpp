#include "Screen.h"
#include "timer_class.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "string.h"

#define TAG "SCREEN"

enum queue_event_type_t
{
    QUEUE_NO_EVENT = 0,
    QUEUE_NEW_TIMER = 1,
};
typedef enum queue_event_type_t queue_event_type_t;

struct screen_queue_data_t
{
    queue_event_type_t type;
    Timer* timer; 
} ;
typedef struct screen_queue_data_t screen_queue_data_t;


Screen<LiquidCrystalGPIO*>::Screen(LiquidCrystalGPIO* lcd){
    set_lcd_object(lcd);
}

Screen<LiquidCrystalGPIO*>::Screen(LiquidCrystalGPIO* lcd,gpio_num_t backlight_gpio){
    set_lcd_object(lcd);
    set_backlight_gpio(backlight_gpio);
}

Screen<LiquidCrystalGPIO*>::~Screen(){
    if (lcd != NULL){
        free(lcd);
    }
    ledc_fade_func_uninstall();
}


void Screen<LiquidCrystalGPIO*>::set_lcd_object(LiquidCrystalGPIO* lcd){
    const int numRows = 2;
    const int numCols = 16;
    this->lcd = lcd;
    this->lcd->begin(numCols, numRows);
    
    // lcd->setCursor(0, 0);
    // lcd->write('H');
    // lcd->setCursor(1,0);
    // lcd->write('E');
    // lcd->setCursor(2, 0);
    // lcd->write('J');
}


void Screen<LiquidCrystalGPIO*>::set_backlight_gpio(gpio_num_t gpio){
    backlight_led = gpio;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT; // resolution of PWM duty
    ledc_timer.timer_num = LEDC_TIMER_1;            // timer index
    ledc_timer.freq_hz = 5000;                      // frequency of PWM signal
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;              // Auto select the source clock
    
    ledc_timer_config(&ledc_timer);

    ledc_channel.gpio_num   = gpio;
    ledc_channel.speed_mode = ledc_timer.speed_mode;
    ledc_channel.channel    = LEDC_CHANNEL_0;
    ledc_channel.timer_sel  = ledc_timer.timer_num;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;

    ledc_channel_config(&ledc_channel);

    // Initialize fade service.
    ledc_fade_func_install(0);

}

void Screen<LiquidCrystalGPIO*>::set_fade_time(uint32_t new_fade_time){
    fade_time = new_fade_time;
}

void Screen<LiquidCrystalGPIO*>::fade_backlight_to(uint32_t value){
    ledc_set_fade_with_time(ledc_channel.speed_mode,
            ledc_channel.channel, value, fade_time);
    ledc_fade_start(ledc_channel.speed_mode,
            ledc_channel.channel, LEDC_FADE_NO_WAIT);

}


void ScreenBase::change_view(screen_views new_view){
    current_view = new_view;
}

void Screen<LiquidCrystalGPIO*>::change_view(screen_views new_view){
    ScreenBase::change_view(new_view);
    lcd->clear();
    if (current_view == ScreenBase::screen_views::CLOCK_WELCOME){
        ESP_LOGI(TAG,"Setting welcome message");
        vTaskDelay(50 / portTICK_PERIOD_MS);
        std::string startup_message = CONFIG_SCREEN_STARTUP_MESSAGE;
        int print_len = startup_message.length()>16?16:startup_message.length();
        for (size_t i = 0; i < print_len; i++)
        {
            lcd->setCursor(i, 0);
            lcd->write(startup_message[i]); 
        }
    }
}

void ScreenBase::update(Timer* timer){
    // ESP_LOGI(TAG, "update Screenbase called");
}


void Screen<LiquidCrystalGPIO*>::update(Timer* timer){
    if (lcd == NULL){
        ESP_LOGW(TAG,"No lcd assigned");
        return;
    }
    // ESP_LOGI("SCREEN", "update GPIO called");

    ScreenBase::update(timer);
    if (current_view == ScreenBase::screen_views::CLOCK_SHOW_TIMER){
        if (timer == NULL){
            return;
        }
        int start_pos_col = 8;
        int start_pos_row = 0;
        char display_string[16];
        // size_t print_len = sprintf(display_string,"%.1f",timer->get_remainder_as_double(TIMER_0));
        size_t print_len = sprintf(display_string,"%s",timer->get_remainder_as_clock(TIMER_0).to_string(true,' ').c_str());

        for (size_t i = 0; i < print_len; i++)
        {
            lcd->setCursor(start_pos_col-i,start_pos_row);
            lcd->write(display_string[print_len-1-i]);
        }
        for (size_t i = print_len; i < 7; i++)
        {
            lcd->setCursor(start_pos_col-i,start_pos_row);
            lcd->write(' ');
        }
    } else if (current_view == ScreenBase::screen_views::CLOCK_WELCOME){
        
    }
    
}

void screen_update_task_function(void* pv){
    ESP_LOGI(TAG,"task start");
    if(pv == NULL){
        ESP_LOGE(TAG,"task function pv was null...");
        while (1)
        {
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        
    }
    ScreenBase* screen = (ScreenBase*)(pv);
    QueueHandle_t screen_queue_handle{NULL};
    screen_queue_handle = screen->screen_queue_handle;
    Timer* timer{NULL};
    int64_t esp_time{esp_timer_get_time()};
    screen_queue_data_t queue_event{};
    while (1)
    {
        while (xQueueReceive(screen_queue_handle, &queue_event, 100/portTICK_PERIOD_MS) == pdPASS){
            if (queue_event.type == QUEUE_NEW_TIMER && queue_event.timer != NULL)
            {
                timer = queue_event.timer;
                // screen->update(timer);
            }         
            // delete queue_event;
            
        /*
        */
        }
        screen->update(timer);
    }
    
}

void ScreenBase::pass_timer_to_task(Timer* timer)
{
    screen_queue_data_t queue_event{QUEUE_NEW_TIMER,timer};
    xQueueSend(screen_queue_handle,&queue_event,portMAX_DELAY);
}

ScreenBase::ScreenBase(){
    // Create semaphore
    screen_queue_handle = xQueueCreate(4,sizeof(screen_queue_data_t));
}

void ScreenBase::start(){
    // Create update task
    ESP_LOGI(TAG,"start");
    xTaskCreate(&screen_update_task_function,"screen_update",2048,(void *)this,5,&this->screen_updater_task_handle);
}

ScreenBase::~ScreenBase(){

}

template<class T>
Screen<T>::Screen(){

}

template<class T>
Screen<T>::~Screen(){
}