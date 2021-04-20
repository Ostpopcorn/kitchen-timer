#ifndef TIMER_SCREEN_MODEL_H
#define TIMER_SCREEN_MODEL_H

#include <cstring>
#include <map>
#include "screen_model_entry_types.h"
#include "battery_display.h"
#include "counter_clock.h"




template<typename object_type_t>
class ScreenModelEntry{
public:
    ScreenModelEntry();
    ScreenModelEntry(object_type_t &new_data);
    ScreenModelEntry(object_type_t &&new_data);

    virtual ~ScreenModelEntry();

    bool set_new(object_type_t &&new_entry);
    bool set_new(object_type_t &new_entry);

    bool value_has_been_updated(bool reset = true) const;

    const object_type_t& get_object() const;

    bool operator==(const ScreenModelEntry<object_type_t> &rhs) const;

private:
    object_type_t current_data{};
    mutable bool has_changed{false};
};

class  ScreenModel{
public:

    ScreenModel();

    virtual ~ScreenModel();

    template<class object_type_t >
    void insert(model_entry_types_t identifier, object_type_t&& value);

    void insert(model_entry_types_t identifier, char* value);
    void insert(model_entry_types_t identifier, const char* value);



    template<class object_type_t >
    const ScreenModelEntry<object_type_t>& get_entry(model_entry_types_t identifier);

    template<class object_type_t >
    const object_type_t& get_entry_object(model_entry_types_t identifier);
    template<class object_type_t >
    const ScreenModelEntry<object_type_t>& operator[](model_entry_types_t identifier);

private:

    std::map<const model_entry_types_t,ScreenModelEntry<int>> integers{};
    std::map<const model_entry_types_t,ScreenModelEntry<std::string>> strings{};
    std::map<const model_entry_types_t,ScreenModelEntry<CounterClock>> clocks{};
    std::pair<model_entry_types_t,ScreenModelEntry<BatteryDisplay>> battery{};
    ScreenModelEntry<BatteryDisplay> dummy_battery{};

};

#include "screen_model.tpp"
#include "screen_model_entry.tpp"

#endif