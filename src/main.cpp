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

    while (1)
    {
        button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
        rotary_encoder_event_t rot_ev = GENERAT_ROTARY_ENCODER_EVENT_T;
        while (xQueueReceive(button_event_queue, &(btn_ev),1) == pdPASS){
            if (btn_ev.pin == GPIO_NUM_27)
            {
                if (btn_ev.event == BUTTON_RISING_EDGE)
                    ESP_LOGI(TAG, "27 rising edge");
                else if (btn_ev.event == BUTTON_FALLING_EDGE)
                    ESP_LOGI(TAG, "27 short press falling edge");
                else if (btn_ev.event == BUTTON_LONG_PRESS)
                    ESP_LOGI(TAG, "27 long press");
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG)
                    ESP_LOGI(TAG, "27 long press falling edge");
            }
            else if ((btn_ev.pin == GPIO_NUM_26) )
            {
                if (btn_ev.event == BUTTON_RISING_EDGE)
                    ESP_LOGI(TAG, "26 rising edge");
                else if (btn_ev.event == BUTTON_FALLING_EDGE)
                    ESP_LOGI(TAG, "26 short press falling edge");
                else if (btn_ev.event == BUTTON_LONG_PRESS)
                    ESP_LOGI(TAG, "26 long press");
                else if (btn_ev.event == BUTTON_FALLING_EDGE_LONG)
                    ESP_LOGI(TAG, "26 long press falling edge");
            }
        }
        //xQueueReceive(button_event_queue, ev,0);
        while (xQueueReceive(rotary_encoder_event_queue, &rot_ev,1) == pdPASS){
            ESP_LOGI(TAG,"%i",rot_ev.state.position);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}