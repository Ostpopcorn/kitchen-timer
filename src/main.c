#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include <stdbool.h>

#include "rotary_encoder.h"
#include "button.h"

#define ROT_ENC_A_GPIO GPIO_NUM_32
#define ROT_ENC_B_GPIO GPIO_NUM_33
#define ROT_ENC_BTN_GPIO GPIO_NUM_27
#define TAG "MAIN"

void app_main()
{
    printf("Hello world!\n");
    ESP_LOGI(TAG, "Start rot");
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    rotary_encoder_info_t rotary_encoder_info = {0};
    // Component init and set to half steps.

    rotary_encoder_init(&rotary_encoder_info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO);

    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&rotary_encoder_info, true));
    QueueHandle_t rotary_encoder_event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&rotary_encoder_info, rotary_encoder_event_queue));

    ESP_LOGI(TAG, "Start button");
    button_info_t button_info = {
        .inverted=false,
        .long_press_time_ms=1000
    };

    gpio_num_t gpio_arr[3] = {GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_NC};
    ESP_ERROR_CHECK(button_init(&button_info, gpio_arr));
    QueueHandle_t button_event_queue = button_create_queue();
    ESP_ERROR_CHECK(button_set_queue(&button_info, button_event_queue));

    printf("INIT done!!\n");
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    while (1)
    {
        button_event_t* ev = malloc(sizeof(button_event_t));
        xQueueReceive(button_event_queue, ev, 100 / portTICK_PERIOD_MS);
        if (ev)
        {
            if ((ev->pin == GPIO_NUM_27) )
            {
                if (ev->event == BUTTON_RISING_EDGE)
                    ESP_LOGI(TAG, "27 rising edge");
                else if (ev->event == BUTTON_FALLING_EDGE)
                    ESP_LOGI(TAG, "27 short press falling edge");
                else if (ev->event == BUTTON_LONG_PRESS)
                    ESP_LOGI(TAG, "27 long press");
                else if (ev->event == BUTTON_FALLING_EDGE_LONG)
                    ESP_LOGI(TAG, "27 long press falling edge");
            }
            else if ((ev->pin == GPIO_NUM_26) )
            {
                if (ev->event == BUTTON_RISING_EDGE)
                    ESP_LOGI(TAG, "26 rising edge");
                else if (ev->event == BUTTON_FALLING_EDGE)
                    ESP_LOGI(TAG, "26 short press falling edge");
                else if (ev->event == BUTTON_LONG_PRESS)
                    ESP_LOGI(TAG, "26 long press");
                else if (ev->event == BUTTON_FALLING_EDGE_LONG)
                    ESP_LOGI(TAG, "26 long press falling edge");
            }
        }
        free(ev);
    }

    ESP_ERROR_CHECK(rotary_encoder_uninit(&rotary_encoder_info));

    printf("Exit\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}