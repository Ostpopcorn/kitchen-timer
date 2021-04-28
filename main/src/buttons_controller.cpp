#include "buttons_controller.h"


ButtonsController::ButtonsController(/* args */)
{
}

ButtonsController::~ButtonsController()
{
    
}

button_info_t* ButtonsController::button_info{nullptr};

void ButtonsController::set_button_info(button_info_t* new_info){
    button_info = new_info;
}

void ButtonsController::update_button()
{
    if(!button_info){
        return;
    }
    if(!button_info->queue){
        return;
    }
    button_event_t btn_ev = GENERATE_BUTTON_EVENT_T;
    while (xQueueReceive((*button_info).queue, &(btn_ev),1) == pdPASS){
        if (btn_ev.pin == button_info->pin_list[0])
        {
            if (callback_button_1){
                buttons_queue.push(button_callback_pair_t{&callback_button_1,btn_ev.event});
            }
        }
        else if (btn_ev.pin == button_info->pin_list[1])
        {
            if (callback_button_2){
                buttons_queue.push(button_callback_pair_t{&callback_button_2,btn_ev.event});
            }
        }
        else if (btn_ev.pin == button_info->pin_list[2])
        {
            if (callback_button_3){
                buttons_queue.push(button_callback_pair_t{&callback_button_3,btn_ev.event});
            }
        }
        else if (btn_ev.pin == button_info->pin_list[3])
        {
            if (callback_button_4){
                buttons_queue.push(button_callback_pair_t{&callback_button_4,btn_ev.event});
            }
        }
    }
}