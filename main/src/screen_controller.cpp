#include "screen_controller.h"
#include "screen_view_16x2.h"

ScreenController::ScreenController()
{
    model= new ScreenModel{};
    ViewBase16x2::assing_model(model);
    model->put_new_entry(ScreenModelEntry::ENTRY_STARTUP,CONFIG_SCREEN_STARTUP_MESSAGE);
    model->put_new_entry(ScreenModelEntry::ENTRY_BATTERY_VOLTAGE,"-.--");
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
void ScreenController::handle_event_timer(TimerContainer* timer)
{
    model->put_new_entry(ScreenModelEntry::ENTRY_PRIMARY_TIMER,
          timer->get_primary_timer()->get_remainder_as_clock().to_string(true,' ').c_str());
}
void ScreenController::handle_event_battery(Battery* battery)
{
    char text[5];
    /*
    memcpy(text,battery->get_last_mesurement_as_string().c_str(),5);
    */
    int val = battery->get_last_mesurement();
    text[0] = '0'+(val/1000)%10;
    text[1] = ':';
    text[2] = '0'+(val/100)%10;
    text[3] = '0'+(val/10)%10;
    text[4] = '\0';
    model->put_new_entry(ScreenModelEntry::ENTRY_BATTERY_VOLTAGE,text);
}