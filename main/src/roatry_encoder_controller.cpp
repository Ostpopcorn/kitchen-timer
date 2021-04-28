#include "roatry_encoder_controller.h"

rotary_encoder_info_t* RotaryEncoderController::rotary_encoder_info{nullptr};

void RotaryEncoderController::set_rotary_encoder_info(rotary_encoder_info_t* new_info){
    rotary_encoder_info = new_info;
}

RotaryEncoderController::RotaryEncoderController(/* args */)
{
    prev_rot_enc_time[0] = 0;
}

RotaryEncoderController::~RotaryEncoderController()
{
}

void RotaryEncoderController::update_rotary_encoder(){
    if(!rotary_encoder_info){
        return;
    }
    if(!rotary_encoder_info->queue){
        return;
    }
    int conv_sum{0};

    rotary_encoder_event_t rot_ev = GENERAT_ROTARY_ENCODER_EVENT_T;

    while (xQueueReceive(rotary_encoder_info->queue, &rot_ev,1) == pdPASS){
        //ESP_LOGI(TAG,"%i",rot_ev.state.position);
        // Make sure there is more than 10 ms between the different events to make 
        // have deacent precision

        for (size_t i = 0; i < mem_len-1; i++)
        {
            prev_rot_enc_pos[mem_len-1-i] = prev_rot_enc_pos[mem_len-1-i-1];
            prev_rot_enc_time[mem_len-1-i]  = prev_rot_enc_time[mem_len-1-i-1]; 
        }

        int64_t curr_micros{esp_timer_get_time()};
        // When there has not been an update in a while we might as well reset the counter of the rotary encoder
        // to make sure it never leaves its bounds.
        int64_t rot_enc_reset_time{10000000};
        // How long it will keep history
        int64_t history_time_length{500000};

        // Rot enc reset condition, too long has passed between the last two events.
        if ((curr_micros-prev_rot_enc_time[0])>(rot_enc_reset_time)){
            // printf("rot reset,%llu",(curr_micros-prev_rot_enc_time[0]));
            rotary_encoder_reset(rotary_encoder_info);
            prev_rot_enc_pos[0]   = 0;
        }else{

            prev_rot_enc_pos[0]   = rot_ev.state.position;
        }
        prev_rot_enc_time[0]  = curr_micros;
        
        
        int len_of_vec_to_use{0};
        for (size_t i = 0; i < mem_len-1; i++)
        {
            // Set one to zero for samples older than 1 sec
            if ( (prev_rot_enc_time[0] - prev_rot_enc_time[i]) > history_time_length ){
                // prev_rot_enc_time[i] = 0;
                len_of_vec_to_use = i;
                // printf("con len is: %i ",i);
                break;
            }
            
        }
        
        
        // Now the previous vector is updated and it can be evaluated
        double conv{0};
        int rot_enc_change{prev_rot_enc_pos[0]-prev_rot_enc_pos[len_of_vec_to_use-1]};
        int rot_enc_change_abs{rot_enc_change*( (int) ((rot_enc_change>=0)?1:-1) )};
        // ESP_LOGI("a","%i %i %i",prev_rot_enc_pos[0],prev_rot_enc_pos[1],prev_rot_enc_pos[2]);
        if (len_of_vec_to_use == 0){
            // printf("ERROR");
        } else if (len_of_vec_to_use == 1){
            if (prev_rot_enc_pos[0]>0){
                conv = 1;
            }else{
                conv = -1;
            }
        } else {
            // First set the amout to scroll by
            if (rot_enc_change_abs>=25){
                conv = 15;
            } else if (rot_enc_change_abs>=15){
                conv = 5;
            }else{
                conv = 1;
            }
            // Then correct its sign
            conv *= ( (double) ((rot_enc_change>=0)?1:-1) );
        }
        conv_sum += static_cast<int>(conv);
        
    }
    // printf("change %i, conv %f\n",rot_enc_change,conv);
    if (conv_sum != 0){   
        rotary_encoder_queue.push(rotary_encoder_callback_pair_t{&callback_rot_changed,conv_sum});
    }
}


