#include "timer_class.h"
#include "string.h"


#define TIMER_SCALE 1000000 // convert counter value to seconds


Timer::Timer()
{
    timers[0].id = 1;
    memcpy(&timers[0].name,"T1\0",3); 
}

void Timer::set_alarm_value(double timer_interval_sec)
{
    timers[0].target_time = timer_interval_sec * TIMER_SCALE;
}

void Timer::set_alarm_value(Clock clock)
{
    timers[0].target_time = clock.get_all_time_as_second() * TIMER_SCALE;
}

void Timer::change_alarm_value(int seconds){
    
    timers[0].target_time += (int64_t) seconds * TIMER_SCALE;
}

void Timer::start()
{
    if (timers[0].t_start == 0){
        timers[0].t_start = esp_timer_get_time();
    }
}

void Timer::pause()
{
    if (timers[0].t_start == 0){

    }else{
        int64_t diff = esp_timer_get_time() - timers[0].t_start;
        timers[0].target_time -= diff;
        timers[0].t_start = 0;
    }
}

double Timer::get_remainder_as_double(timer_idx_t timer_idx)
{
    int64_t curr_time{esp_timer_get_time()};
    if (timers[0].t_start == 0)
    {
        // Timer is not running
        return ( (double) timers[0].target_time) /TIMER_SCALE;
    }else{
        // Timer is running
        return ((double)(timers[0].t_start + timers[0].target_time - curr_time))/TIMER_SCALE;
    }
    
}

Clock Timer::get_remainder_as_clock(timer_idx_t timer_idx){
    Clock clock;
    clock = clock + (int) this->get_remainder_as_double(timer_idx)+0.5;
    // printf("%s\n",clock.to_string().c_str());
    return clock;
}