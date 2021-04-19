#ifndef ESP_TIMER_BATTERY_DISPLAY_OBJECT_H
#define ESP_TIMER_BATTERY_DISPLAY_OBJECT_H

#include <string>

class BatteryDisplay{
/*
 * Holds the last measurment value and has a to_string method which formats the string.
 */
public:
    BatteryDisplay();
    BatteryDisplay(int measurement);
    std::string to_string() const;
    bool operator==(const BatteryDisplay rhs){
        return measurement == rhs.get_measurement();
    }
    int get_measurement() const;
private:
    int measurement{0};

};



#endif