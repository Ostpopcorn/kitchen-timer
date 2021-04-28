#pragma once

#include "functional"

class Animator
{
public:
    typedef std::function<int64_t()> get_clock_function_t;
    // typedef void (*get_clock_function_t)();
    static void set_clock_func(get_clock_function_t);
    void set_private_clock_func(get_clock_function_t);
    bool value_has_been_updated(bool reset = true) const;
protected:
    int64_t get_time();
    int64_t get_time_diff_since_reset();
    void time_diff_reset();
    mutable bool has_updated{false};
private:
    static get_clock_function_t static_clock_func; 
    get_clock_function_t private_clock_func; 
    int64_t last_timer_time{0};

public:
    Animator();
    virtual ~Animator();
};

class AnimationSpinningWheel : public Animator
{
public:
    void update();
    const char* get_char_pointer();
    char get_char();
private:
    static const int total_animation_length{1000000}; // 1 000 ms
    char current_char{' '};
    int64_t time_per_frame{total_animation_length/4};
};

template<char c1, char c2>
class AnimationBlinkTwoChar : public Animator
{
public:
    void update();
    const char* get_char_pointer();
    char get_char();
private:
    static const int total_animation_length{2*500000}; // 500 ms
    char current_char{c1};
    int64_t time_per_frame{total_animation_length/2};
};

#include "animator.tpp"