#include "battery_display.h"

/* 
std::string Battery::get_last_mesurement_as_string() 
{
    char text[5];
    text[0] = (last_measure/1000)%10;
    text[1] = ':';
    text[2] = (last_measure/100)%10;
    text[3] = (last_measure/10)%10;
    text[4] = '\n';
    return std::string(text); 
    //model->put_new_entry(ScreenModelEntry::ENTRY_BATTERY_VOLTAGE,text);

}
*/
BatteryDisplay::BatteryDisplay():measurement{0}{

}
BatteryDisplay::BatteryDisplay(int measurement): measurement{measurement}{

}
int BatteryDisplay::get_measurement() const{
    return measurement;
}
std::string BatteryDisplay::to_string() const {
    
    char text[5];
    /*
    memcpy(text,battery->get_last_mesurement_as_string().c_str(),5);
    */
    text[0] = '0'+(measurement/1000)%10;
    text[1] = ':';
    text[2] = '0'+(measurement/100)%10;
    text[3] = '0'+(measurement/10)%10;
    text[4] = '\0';
    return std::string(text);
}
