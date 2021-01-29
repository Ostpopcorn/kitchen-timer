#ifndef ESP_TIMER_CLASS_H
#define ESP_TIMER_CLASS_H

#include "esp_types.h"
#include <functional>
#include "clock.h"
#include "string.h"
#include "esp_timer.h"


class TimerContainer{
public:
    typedef enum{
        EVENT_TYPE_NON = 0,
        EVENT_TYPE_ALARM = 1,
        EVENT_TYPE_START,
        EVENT_TYPE_STOP,
        EVENT_TYPE_RESET,
    }timer_event_type_t;

    typedef uint8_t timer_id_t;
    class Timer;

    struct timer_event_t {
        TimerContainer::Timer* timer_info;
        TimerContainer::timer_event_type_t event_type;
    };
    
    //typedef void (*alarm_callback_t)(struct timer_event_t const);
    typedef std::function<void(struct timer_event_t)> alarm_callback_t;

    class Timer{
    private:
        timer_id_t id{0};
        std::string name{""};
        int t_start{0};
        int target_time{0};
        bool alarm_triggerd{false};

    public:
        Timer();
        Timer(timer_id_t);
        Timer(timer_id_t,std::string);

        std::string get_name() const;

        alarm_callback_t callback_timer_start{NULL};
        alarm_callback_t callback_timer_stop{NULL};
        alarm_callback_t callback_timer_alarm{NULL};
        alarm_callback_t callback_timer_reset{NULL};
        void start();
        void pause();
        void set_alarm_value(double timer_interval_sec);
        void set_alarm_value(Clock timer_interval_sec);
        double get_remainder_as_double();
        Clock get_remainder_as_clock();
        void change_alarm_value(int seconds);
        void check_alarm();
        bool is_running();

    };
private:
    static const timer_id_t max_timers{16};
    Timer* get_timer(timer_id_t timer_id);
    int64_t t_0{esp_timer_get_time()};
    Timer timers[max_timers];
    timer_id_t primary_id{1};
public:
    TimerContainer();
    void update();
    Timer* get_primary_timer();
    void register_callback(timer_id_t timer_id,timer_event_type_t event_type, alarm_callback_t new_callback);
};


#endif