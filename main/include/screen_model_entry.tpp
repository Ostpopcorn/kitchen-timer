#include "screen_model.h"

template<typename object_type_t>
ScreenModelEntry<object_type_t>::ScreenModelEntry(){}


template<typename object_type_t>
ScreenModelEntry<object_type_t>::ScreenModelEntry(object_type_t &&new_data):current_data{new_data}, has_changed{true}{}

template<typename object_type_t>
ScreenModelEntry<object_type_t>::ScreenModelEntry(object_type_t &new_data):current_data{new_data}, has_changed{true}{}


template<typename object_type_t>
ScreenModelEntry<object_type_t>::~ScreenModelEntry() {

}

template<typename object_type_t>
bool ScreenModelEntry<object_type_t>::set_new(object_type_t &&new_entry){
    if (current_data == new_entry){
        return false;
    }
    current_data = new_entry;
    has_changed = true;
    return true;
}
template<typename object_type_t>
bool ScreenModelEntry<object_type_t>::set_new(object_type_t &new_entry){
    if (current_data == new_entry){
        return false;
    }
    current_data = new_entry;
    has_changed = true;
    return true;
}

template<typename object_type_t>
bool ScreenModelEntry<object_type_t>::value_has_been_updated(bool reset) const{
    bool return_bool = has_changed;
    if (reset){
        has_changed = false;
    }
    return return_bool;
}


template<typename object_type_t>
const object_type_t& ScreenModelEntry<object_type_t>::get_object() const{
    return current_data;
}

template<typename object_type_t>
bool ScreenModelEntry<object_type_t>::operator==(const ScreenModelEntry<object_type_t> &rhs) const {
    return this->current_data == rhs.current_data;
}

