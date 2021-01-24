#include "timer_class.h"
#include "string.h"


#define TIMER_SCALE 1000000 // convert counter value to seconds


TimerContainer::TimerContainer()
{
    char buffer[16];
    for (size_t i = 0; i < max_timers; i++)
    {
        sprintf(buffer,"Timer %i",i);
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

void TimerContainer::Timer::set_alarm_value(double timer_interval_sec)
{
    target_time = timer_interval_sec * TIMER_SCALE;
}

void TimerContainer::Timer::set_alarm_value(Clock clock)
{
    target_time = clock.get_all_time_as_second() * TIMER_SCALE;
}

void TimerContainer::Timer::change_alarm_value(int seconds){
    
    target_time += (int64_t) seconds * TIMER_SCALE;
}

void TimerContainer::Timer::start()
{
    if (!is_running()){
        t_start = esp_timer_get_time();
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
    return t_start != 0;
}
void TimerContainer::Timer::pause()
{
    if (is_running())
    {
        int64_t diff = esp_timer_get_time() - t_start;
        target_time -= diff;
        t_start = 0;
        if(callback_timer_stop != NULL){
            timer_event_t timer_event{
                .timer_info = this,
                .event_type = EVENT_TYPE_STOP
            };
            callback_timer_stop(timer_event);
        }
    }
}

void TimerContainer::Timer::check_alarm(){
    if (alarm_triggerd == true){
        // Already tripped 
        return;
    }

    int64_t curr_time{esp_timer_get_time()};
    int64_t diff = t_start + target_time - curr_time;
    if (diff>0){
        // Not tripped yet
    }
    else
    {
        alarm_triggerd = true;
        timer_event_t timer_event{
            .timer_info = this,
            .event_type = EVENT_TYPE_ALARM
        };
        callback_timer_alarm(timer_event);
        return;
    }
}


double  TimerContainer::Timer::get_remainder_as_double()
{
    int64_t curr_time{esp_timer_get_time()};
    if (t_start == 0)
    {
        // Timer is not running
        return ( (double) target_time) /TIMER_SCALE;
    }else{
        // Timer is running
        return ((double)(t_start + target_time - curr_time))/TIMER_SCALE;
    }
    
}

Clock TimerContainer::Timer::get_remainder_as_clock(){
    Clock clock;
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
        get_timer(i)->check_alarm();
    }
    
}

void TimerContainer::register_callback(timer_id_t timer_id, timer_event_type_t event_type, void  (*new_callback)(timer_event_t const timer_event)){
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
    case EVENT_TYPE_ALARM:
        current_timer->callback_timer_alarm = new_callback;
        break;
    case EVENT_TYPE_RESET:
        current_timer->callback_timer_reset = new_callback;
        break;
    
    default:
        break;
    }
    return;
}
