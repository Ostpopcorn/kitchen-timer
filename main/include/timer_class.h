#ifndef ESP_TIMER_CLASS_H
#define ESP_TIMER_CLASS_H

#include "esp_types.h"
#include <functional>
#include "counter_clock.h"
#include "string.h"
#include "esp_timer.h"


class TimerContainer{
public:
    /*
     * ALARM: Alarm has triggerd and hopefully something like a sound is sounding
     * START: Clock was in stop or paus and now starts to counting
     * STOP : Clock stops runnning.
     * Reset: Clock loses all memories and set time to 00:00
     */
    typedef enum{
        EVENT_TYPE_NON = 0,
        EVENT_TYPE_ALARM_TRIGGERD = 1,
        EVENT_TYPE_ALARM_OFF ,
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
    public:
        typedef enum{
            STATE_STOP = 1,
            STATE_DOWNCOUNTING,
            STATE_UPCOUNTING,
            STATE_ALARM_TRIGGERD,
        } timer_state_t;

    private:
        timer_id_t id{0};
        std::string name{""};
        timer_state_t current_state;
        int64_t t_start{0}; // These are defined as ms in code
        int64_t remaining_target_time{0};
        int64_t total_target_time{0};
        int64_t get_scaled_time();

    public:
        Timer();
        Timer(timer_id_t);
        Timer(timer_id_t,std::string);

        std::string get_name() const;
        timer_state_t get_state() const;

        alarm_callback_t callback_timer_start{NULL};
        alarm_callback_t callback_timer_stop{NULL};
        alarm_callback_t callback_timer_alarm_triggerd{NULL};
        alarm_callback_t callback_timer_alarm_off{NULL};
        alarm_callback_t callback_timer_reset{NULL};

        void start();
        void stop();
        void reset();

        void set_alarm_value(double timer_interval_sec);
        void set_alarm_value_ms(int64_t timer_interval_msec);
        void set_alarm_value(Clock timer_interval_sec);

        double get_remainder_as_double();
        CounterClock get_remainder_as_clock();
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