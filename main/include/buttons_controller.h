#ifndef BUTTONS_CONTROLLER_H
#define BUTTONS_CONTROLLER_H

#include "button.h"
#include <queue>
#include <functional>




/*
 * Each view should have a ButtonsController with assigned functions.
 * 
 */

class ButtonsControllerBase{
    
    virtual void update_button(){};
};

class ButtonsController : public ButtonsControllerBase
{
public:
    typedef std::function<void()> button_callback_queue_t;
    typedef std::function<void(button_state_t)> button_callback_t;
    typedef std::function<void(button_state_t)> rotary_encoder_callback_t;

    button_callback_t callback_button_1{};
    button_callback_t callback_button_2{};
    button_callback_t callback_button_3{};
    button_callback_t callback_button_4{};
    rotary_encoder_callback_t callback_rot_changed{};

private:
    static button_info_t* button_info;

    std::queue<button_callback_queue_t> buttons_queue{};

public:
    std::queue<button_callback_queue_t>& get_buttons_queue(){return buttons_queue;};
    ButtonsController(/* args */);
    ~ButtonsController();
    static void set_button_info(button_info_t* new_info);
    virtual void update_button();
};


#endif