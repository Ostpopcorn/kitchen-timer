#include "animator.h"

Animator::get_clock_function_t Animator::clock_func{};

Animator::Animator()
{
    time_diff_reset();
}

Animator::~Animator()
{

}

void Animator::set_clock_func(get_clock_function_t new_func){
    if(new_func){
        Animator::clock_func = new_func;
    }
}
int64_t Animator::get_time(){
    if (Animator::clock_func){
        return Animator::clock_func();
    }
    return 0;
}

int64_t Animator::get_time_diff_since_reset(){
    return get_time()-last_timer_time;
}

void Animator::time_diff_reset(){
    last_timer_time = clock_func();
}


void AnimationSpinningWheel::update(){
    int64_t diff = get_time_diff_since_reset();
    
    if (diff >(4*time_per_frame))
    {
        // reset animation condition
        time_diff_reset();
    }
    if (diff >(3*time_per_frame))
    {
        current_char = ' ';
    } else if (diff >(2*time_per_frame))
    {
        // reset animation condition
        current_char = '^';
    } else if (diff >(1*time_per_frame))
    {
        // reset animation condition
        current_char = '-';
    } else if (diff >(0*time_per_frame))
    {
        // reset animation condition
        current_char = '_';
    } 
}

char AnimationSpinningWheel::get_char(){
    return current_char;
}
const char* AnimationSpinningWheel::get_char_pointer(){
    return &current_char;
}