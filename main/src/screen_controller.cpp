#include "screen_controller.h"

ScreenController::ScreenController()
{
    model= new ScreenModel{};
    ViewBase16x2::assing_model(model);
    model->put_new_entry(ScreenModelEntry::ENTRY_STARTUP,CONFIG_SCREEN_STARTUP_MESSAGE);
    
}

ScreenController::~ScreenController()
{
}

void ScreenController::update(){
    current_view->update();
}
void ScreenController::change_view(screen_views_t new_view){
    switch (new_view)
    {
    case CLOCK_BLANK:
        // Dafaq
        break;
    case CLOCK_WELCOME:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2Start{};
        break;
    case CLOCK_SHOW_TIMER:
        if (current_view != NULL){
            delete current_view;
        }
        current_view = new View16x2SimpleClock{};
        break;
    default:
        break;
    }
    update();
}
void ScreenController::handle_event_timer(Timer* timer)
{
    model->put_new_entry(ScreenModelEntry::ENTRY_PRIMARY_TIMER,
          timer->get_remainder_as_clock(TIMER_0).to_string(true,' ').c_str());
}