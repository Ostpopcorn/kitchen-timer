#include "timer_class.h"
#include "string.h"


#define TIMER_SCALE 1000 // convert counter value to seconds


TimerContainer::TimerContainer()
{
    char buffer[16];
    for (size_t i = 0; i < max_timers; i++)
    {
        sprintf(buffer,"T%i",i);
        timers[i] = Timer(i,buffer);
    }
}

TimerContainer::Timer::Timer(){

}

TimerContainer::Timer::Timer(timer_id_t timer_id): id{timer_id}{

}

TimerContainer::Timer::Timer(timer_id_t timer_id, std::string name): id{timer_id}, name{name}
{
    
}


std::string TimerContainer::Timer::get_name() const {
    return name;
}

TimerContainer::Timer::timer_state_t TimerContainer::Timer::get_state() const{
    return current_state;
}

int64_t TimerContainer::Timer::get_scaled_time()
{
    return esp_timer_get_time()/1000;
}

void TimerContainer::Timer::set_alarm_value(double timer_interval_sec)
{
    total_target_time = timer_interval_sec * TIMER_SCALE;
    remaining_target_time = total_target_time;
}

void TimerContainer::Timer::set_alarm_value_ms(int64_t timer_interval_msec)
{
    total_target_time = timer_interval_msec;
    remaining_target_time = total_target_time;
}

void TimerContainer::Timer::set_alarm_value(Clock clock)
{
    total_target_time = clock.get_all_time_as_second() * TIMER_SCALE;
    remaining_target_time = total_target_time;
}

void TimerContainer::Timer::change_alarm_value(int seconds){
    int s = seconds * TIMER_SCALE;
    total_target_time +=  s;
    remaining_target_time +=  s;
}

void TimerContainer::Timer::start()
{
    if (!is_running()){
        t_start = get_scaled_time();
        current_state = STATE_DOWNCOUNTING;
        if(callback_timer_start != NULL){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_START
            };
            callback_timer_start(timer_event);
        }
    }
}

bool TimerContainer::Timer::is_running()
{
    switch (current_state)
    {
    case STATE_DOWNCOUNTING:
    case STATE_UPCOUNTING:
    case STATE_ALARM_TRIGGERD:
        return true;
    
    default:
        return false;
    }
}
void TimerContainer::Timer::stop()
{
    if (!is_running())
    {
        return;
    }
    int64_t diff = get_scaled_time() - t_start;
    remaining_target_time -= diff;
    t_start = 0;
    if (current_state == STATE_ALARM_TRIGGERD){
        // Reset remaing time to the entered time
        remaining_target_time = total_target_time;
        if(callback_timer_alarm_off){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_ALARM_OFF
            };
            callback_timer_alarm_off(timer_event);
        }
    }else{
        if(callback_timer_stop){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_STOP
            };
            callback_timer_stop(timer_event);
        }
    }
    current_state = STATE_STOP;
}
void TimerContainer::Timer::reset()
{
    remaining_target_time = total_target_time;
    t_start = 0;
    if (current_state == STATE_ALARM_TRIGGERD){
        // Reset remaing time to the entered time
        if(callback_timer_alarm_off){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_ALARM_OFF
            };
            callback_timer_alarm_off(timer_event);
        }
    }

    if(callback_timer_reset){
        timer_event_t timer_event{
            .timer_info = this,
            .event_type = EVENT_TYPE_RESET
        };
        callback_timer_reset(timer_event);
    }
    current_state = STATE_STOP;
}

void TimerContainer::Timer::check_alarm(){
    // Already tripped or stopped
    if (current_state == STATE_ALARM_TRIGGERD||
        current_state == STATE_STOP){
        return;
    }

    int64_t curr_time{get_scaled_time()};
    int64_t diff = t_start + remaining_target_time - curr_time;
    // Should trip if time remaining is negative
    if (diff<=0)
    {
        current_state = STATE_ALARM_TRIGGERD;
        if(callback_timer_alarm_triggerd != NULL){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_ALARM_TRIGGERD
            };
            callback_timer_alarm_triggerd(timer_event);
        }
    }
    return;
}


double  TimerContainer::Timer::get_remainder_as_double()
{
    int64_t curr_time{get_scaled_time()};
    if (is_running())
    {
        // Timer is running
        return ((double)(t_start + remaining_target_time - curr_time))/TIMER_SCALE;
    }else{
        // Timer is not running
        return ( (double) remaining_target_time) /TIMER_SCALE;
    }
    
}

CounterClock TimerContainer::Timer::get_remainder_as_clock(){
    CounterClock clock;
    clock = clock + (int) this->get_remainder_as_double()+0.5;
    // printf("%s\n",clock.to_string().c_str());
    return clock;
}

TimerContainer::Timer* TimerContainer::get_timer(timer_id_t timer_id){
    return &timers[timer_id];
}

TimerContainer::Timer* TimerContainer::get_primary_timer(){
    return get_timer(primary_id);
}

void TimerContainer::update(){
    for (timer_id_t i = 0; i < max_timers; i++)
    {
        auto * t = get_timer(i);
        if(t){
            t->check_alarm();
        }
    }
    
}

void TimerContainer::register_callback(timer_id_t timer_id, timer_event_type_t event_type, alarm_callback_t new_callback){
    if (new_callback == NULL){
        return;
    }
    Timer* current_timer = get_timer(timer_id);
    switch (event_type)
    {
    case EVENT_TYPE_START:
        current_timer->callback_timer_start = new_callback;
        break;
    case EVENT_TYPE_STOP:
        current_timer->callback_timer_stop = new_callback;
        break;
    case EVENT_TYPE_ALARM_TRIGGERD:
        current_timer->callback_timer_alarm_triggerd = new_callback;
        break;
    case EVENT_TYPE_ALARM_OFF:
        current_timer->callback_timer_alarm_off = new_callback;
        break;
    case EVENT_TYPE_RESET:
        current_timer->callback_timer_reset = new_callback;
        break;
    
    default:
        break;
    }
    return;
}
