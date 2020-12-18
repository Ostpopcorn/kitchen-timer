#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

class Timer{
    xQueueHandle queue_handle;
    timer_idx_t timer_idx = TIMER_0;
    void init();
public:
    Timer();
    void set_alarm_value(double timer_interval_sec);
    void start();
};


#endif