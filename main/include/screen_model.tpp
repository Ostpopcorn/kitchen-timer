#include "screen_model.h"
#include "custom_exception.h"
#include "esp_log.h"
/*
template<class identifier_t >
template<class object_type_t >
ScreenModelEntry<object_type_t>& ScreenModel<identifier_t>::get_entry(const identifier_t& identifier) {
    return all_entries[identifier];
}
// object_type_t get_entry_object(const identifier_t& identifier);
template<class identifier_t >
template<class object_type_t >
const object_type_t& ScreenModel<identifier_t>::get_entry_object(const identifier_t& identifier) {
    exception_throw();
}
*/



template<class object_type_t >
void ScreenModel::insert(model_entry_types_t identifier, object_type_t&& value) {
    object_type_t a{value};
    ESP_LOGI("Model","gen insert");
    exception_throw();
}

template<class object_type_t >
const ScreenModelEntry<object_type_t>& ScreenModel::get_entry(model_entry_types_t identifier) {
    exception_throw();
    ScreenModelEntry<object_type_t> ret{};
    return ret;
}

template<class object_type_t >
const object_type_t& ScreenModel::get_entry_object(model_entry_types_t identifier) {
    return get_entry<object_type_t>(identifier).get_object();
}

template<class object_type_t>
const ScreenModelEntry<object_type_t> &ScreenModel::operator[](model_entry_types_t identifier) {
    return get_entry<object_type_t>(identifier);
}
