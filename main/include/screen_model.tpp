#include "screen_model.h"

template<class identifier_t,class object_type_t >
ScreenModelEntry<identifier_t,object_type_t>::ScreenModelEntry(){}

template<class identifier_t,class object_type_t >
ScreenModelEntry<identifier_t,object_type_t>::ScreenModelEntry(identifier_t identifier): identifier{identifier}{}

template<class identifier_t,class object_type_t >
ScreenModelEntry<identifier_t,object_type_t>::ScreenModelEntry(identifier_t identifier, object_type_t value):
        identifier{identifier},current_object{value}{}

template<class identifier_t,class object_type_t >
bool ScreenModelEntry<identifier_t,object_type_t>::operator<(ScreenModelEntry const &rhs) const {
    return (this->get_identifier() < rhs.get_identifier());
}

template<class identifier_t,class object_type_t >
bool ScreenModelEntry<identifier_t,object_type_t>::operator!=(ScreenModelEntry const &rhs) const {
    return (this->get_identifier() != rhs.get_identifier());
}

template<class identifier_t,class object_type_t >
void ScreenModelEntry<identifier_t,object_type_t>::set_new(object_type_t new_entry){
    if (new_entry == current_object){
        return;
    }
    current_object = new_entry;
    has_changed = true;
    return;
}

template<class identifier_t,class object_type_t >
bool ScreenModelEntry<identifier_t,object_type_t>::has_been_updated(bool reset){
    bool return_bool = has_changed;
    if (reset == true){
        has_changed = false;
    }
    return return_bool;
}

template<class identifier_t,class object_type_t >
object_type_t ScreenModelEntry<identifier_t,object_type_t>::get_object(){
    return current_object;
}

template<class identifier_t,class object_type_t >
identifier_t ScreenModelEntry<identifier_t,object_type_t>::get_identifier() const {
    return identifier;
}


template<class identifier_t,class object_type_t >
ScreenModelEntry<identifier_t,object_type_t>& ScreenModel<identifier_t,object_type_t>::get_entry(identifier_t identifier) {

    //std::map<identifier_t,ScreenModelEntry<identifier_t,object_type_t>>::iterator
    auto search = all_entries.find(identifier);
    if (search != all_entries.end()) {
        // ScreenModel::ScreenModelEntry< a = search;
        return search->second;
    } else {
        return empty;
    }
}

template<class identifier_t,class object_type_t >
object_type_t ScreenModel<identifier_t,object_type_t>::get_entry_object(identifier_t identifier) {
    return get_entry(identifier).get_object();
}

template<class identifier_t,class object_type_t >
ScreenModel<identifier_t,object_type_t>::ScreenModel(){

}

template<class identifier_t,class object_type_t >
void ScreenModel<identifier_t,object_type_t>::put_new_entry(identifier_t identifier, object_type_t value) {
    put_new_entry(ScreenModelEntry<identifier_t,object_type_t>{identifier,value});
}

template<class identifier_t,class object_type_t >
void ScreenModel<identifier_t,object_type_t>::put_new_entry(ScreenModelEntry<identifier_t,object_type_t> new_entry) {
    typename std::map<identifier_t,ScreenModelEntry<identifier_t,object_type_t>>::iterator
            search = all_entries.find(new_entry.get_identifier());
    if (search != all_entries.end()) {
        // ScreenModel::ScreenModelEntry< a = search;
        ScreenModelEntry<identifier_t,object_type_t>& res = search->second;
        res.set_new(new_entry.get_object());
    } else {
        // ESP_LOGE("SCREENMODEL","NOT key %i",identifier);
        all_entries.insert(std::pair<identifier_t,ScreenModelEntry<identifier_t,object_type_t>>(new_entry.get_identifier(),new_entry));
    }
}

