#ifndef ESP_TIMER_CLASS_H
#define ESP_TIMER_CLASS_H

#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "clock.h"

typedef struct {
    int type;  // the type of timer's event
    timer_group_t timer_group;
    timer_idx_t timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

#define GENERAT_TIMER_EVENT_T {.timer_group = TIMER_GROUP_MAX, .timer_idx  = TIMER_MAX, .timer_counter_value = 0}

class Timer{
    timer_idx_t timer_idx = TIMER_0;
    xQueueHandle queue_handle;
    void init();
public:
    Timer();
    xQueueHandle get_queue_handle();
    void set_alarm_value(double timer_interval_sec);
    void set_alarm_value(Clock timer_interval_sec);
    void change_alarm_value(int seconds);
    void start();
    void pause();
    double get_remainder_as_double(timer_idx_t timer_idx);
    Clock get_remainder_as_clock(timer_idx_t timer_idx);
};


#endif