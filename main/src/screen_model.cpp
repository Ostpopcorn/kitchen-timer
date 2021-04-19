#include "screen_model.h"
#include "custom_exception.h"
#include "esp_log.h"
#include "battery_monitor.h"

ScreenModel::ScreenModel(){

}
ScreenModel::~ScreenModel() {

}

// "Vanlig funktioner"
void ScreenModel::insert(model_entry_types_t identifier, const char* value) {
    strings[identifier].set_new(std::string(value));
}

void ScreenModel::insert(model_entry_types_t identifier, char* value) {
    insert(identifier,(const char*) value);
}
// Int
template<>
void ScreenModel::insert(model_entry_types_t identifier, int &&value) {
    integers[identifier].set_new(value);
}

// String ref 
template<>
void ScreenModel::insert(model_entry_types_t identifier, std::string &&value) {
    strings[identifier].set_new(value);
}
// CounterClock
template<>
void ScreenModel::insert(model_entry_types_t identifier, Clock_display &&value) {
    clocks[identifier].set_new(value);
}
// Battery_Value
template<>
void ScreenModel::insert(model_entry_types_t identifier, BatteryDisplay &&value) {
    if (battery.first) {
        if(battery.first == identifier ){
            battery.second.set_new(value);
        }else{
            exception_throw();
        }
    } else{
        ScreenModelEntry<BatteryDisplay> bat{value};
        battery = std::pair<model_entry_types_t,ScreenModelEntry<BatteryDisplay>>{identifier, bat};
    }
}


template< >
const ScreenModelEntry<int>& ScreenModel::get_entry(model_entry_types_t identifier) {
    return integers[identifier];
}
template< >
const ScreenModelEntry<std::string>& ScreenModel::get_entry(model_entry_types_t identifier) {
    return strings[identifier];
}
template< >
const ScreenModelEntry<Clock_display>& ScreenModel::get_entry(model_entry_types_t identifier) {
    return clocks[identifier];
}
template<>
const ScreenModelEntry<BatteryDisplay>& ScreenModel::get_entry(model_entry_types_t identifier) {
    if (identifier == battery.first){
        return battery.second;
    }
    return dummy_battery;
}