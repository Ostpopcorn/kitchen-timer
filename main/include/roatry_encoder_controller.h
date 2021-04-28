#ifndef ROTARY_ENCODER_CONTROLLER_H
#define ROTARY_ENCODER_CONTROLLER_H

#include <functional>
#include <queue>
#include "rotary_encoder.h"

class RotaryEncoderControllerBase{
    virtual void update_rotary_encoder(){};
};

class RotaryEncoderController : public RotaryEncoderControllerBase
{
public:
    typedef std::function<void(int)> rotary_encoder_callback_t;
    typedef std::pair<rotary_encoder_callback_t*,int> rotary_encoder_callback_pair_t;
    rotary_encoder_callback_t callback_rot_changed{};
private:
    static rotary_encoder_info_t* rotary_encoder_info;
    std::queue<rotary_encoder_callback_pair_t> rotary_encoder_queue{};

    static const int32_t mem_len = 32;
    int32_t prev_rot_enc_pos[mem_len];
    uint64_t prev_rot_enc_time[mem_len];
public:
    std::queue<rotary_encoder_callback_pair_t>& get_rotary_encoder_queue(){return rotary_encoder_queue;};
    RotaryEncoderController(/* args */);
    ~RotaryEncoderController();
    virtual void update_rotary_encoder();
    
    static void set_rotary_encoder_info(rotary_encoder_info_t* new_info);
};



#endif