#include "animator.h"

template<char c1, char c2>
void AnimationBlinkTwoChar<c1,c2>::update(){
    int64_t diff = get_time_diff_since_reset();
    
    if (diff >(2*time_per_frame))
    {
        // reset animation condition
        time_diff_reset();
    }
    if (diff >(1*time_per_frame))
    {
        // reset animation condition
        current_char = c2;
    } else if (diff >(0*time_per_frame))
    {
        // reset animation condition
        current_char = c1;
    } 
}

template<char c1, char c2>
char AnimationBlinkTwoChar<c1,c2>::get_char(){
    return current_char;
}
template<char c1, char c2>
const char* AnimationBlinkTwoChar<c1,c2>::get_char_pointer(){
    return &current_char;
}