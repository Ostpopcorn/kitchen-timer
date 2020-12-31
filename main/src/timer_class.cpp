#include "timer_class.h"

#define TIMER_DIVIDER 16                             //  Hardware timer clock divider
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER) // convert counter value to seconds

// xQueueHandle queue_handle;

typedef struct
{
    timer_idx_t timer_id;
    xQueueHandle queue;
} timer_isr_input_data_t;

void IRAM_ATTR timer_group0_isr(void *para)
{
    timer_spinlock_take(TIMER_GROUP_0);

    timer_isr_input_data_t *input_data = (timer_isr_input_data_t *)para;

    timer_idx_t timer_idx = input_data->timer_id;
    /* Retrieve the interrupt status and the counter value
       from the timer that reported the interrupt */
    uint32_t timer_intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
    uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer_idx);

    /* Prepare basic event data
       that will be then sent back to the main program task */
    timer_event_t evt;
    evt.timer_group = TIMER_GROUP_0;
    evt.timer_idx = timer_idx;
    evt.timer_counter_value = timer_counter_value;

    /* Clear the interrupt
       and update the alarm time for the timer with without reload */
    if (timer_intr & TIMER_INTR_T0)
    {
        timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
        timer_counter_value += (uint64_t)(1 * TIMER_SCALE);
        timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, timer_idx, timer_counter_value);
    }
    else
    {
    }

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(input_data->queue, &evt, NULL);
    timer_spinlock_give(TIMER_GROUP_0);
}

Timer::Timer()
{
    init();
}

xQueueHandle Timer::get_queue_handle()
{
    return this->queue_handle;
}

void Timer::init()
{
    this->queue_handle = xQueueCreate(4, sizeof(timer_event_t));

    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .divider = TIMER_DIVIDER,
    }; // default clock source is APB
    timer_init(TIMER_GROUP_0, timer_idx, &config);
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    timer_isr_input_data_t *data_for_isr;
    data_for_isr = new timer_isr_input_data_t{
        .timer_id = this->timer_idx,
        .queue = this->queue_handle,
    };

    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *)data_for_isr, ESP_INTR_FLAG_IRAM, NULL);
}

void Timer::set_alarm_value(double timer_interval_sec)
{
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
}

void Timer::set_alarm_value(Clock clock)
{
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, clock.get_all_time_as_second() * TIMER_SCALE);
}

void Timer::change_alarm_value(int seconds){
    uint64_t alarm_value{0};
    timer_get_alarm_value(TIMER_GROUP_0, this->timer_idx, &alarm_value);
    alarm_value += (uint64_t) seconds * TIMER_SCALE;
    timer_set_alarm_value(TIMER_GROUP_0, this->timer_idx, alarm_value);
}

void Timer::start()
{
    timer_start(TIMER_GROUP_0, timer_idx);
}

void Timer::pause()
{
    timer_pause(TIMER_GROUP_0, timer_idx);
}

double Timer::get_remainder_as_double(timer_idx_t timer_idx)
{
    uint64_t alarm_value{0};
    uint64_t current_value{0};

    timer_get_alarm_value(TIMER_GROUP_0, this->timer_idx, &alarm_value);
    timer_get_counter_value(TIMER_GROUP_0, this->timer_idx, &current_value);

    double curr{((double) current_value)/TIMER_SCALE };
    double alarm{((double) alarm_value)/TIMER_SCALE };
    return alarm - curr;
}

Clock Timer::get_remainder_as_clock(timer_idx_t timer_idx){
    Clock clock;
    clock = clock + (int) this->get_remainder_as_double(timer_idx)+0.5;
    // printf("%s\n",clock.to_string().c_str());
    return clock;
}