#include "screen_model.h"


ScreenModelEntry::ScreenModelEntry(): identifier{ScreenModelEntry::ENTRY_NON}{}

ScreenModelEntry::ScreenModelEntry(ScreenModelEntry::model_entry_types_t identifier): identifier{identifier}{}

ScreenModelEntry::ScreenModelEntry(ScreenModelEntry::model_entry_types_t identifier, std::string value):
        identifier{identifier},current_string{value}{}

bool ScreenModelEntry::operator<(ScreenModelEntry const &rhs) const {
    return (this->get_identifier() < rhs.get_identifier());
}

bool ScreenModelEntry::operator!=(ScreenModelEntry const &rhs) const {
    return (this->get_identifier() != rhs.get_identifier());
}

template <>
void ScreenModelEntry::set_new(std::string new_entry_string){
    // std::string new_entry_string{new_entry.to_string()};
    if (new_entry_string == current_string){
        return;
    }
    current_string = new_entry_string;
    has_changed = true;
    return;
}

template <class T>
void ScreenModelEntry::set_new(T new_entry){
    std::string new_entry_string{new_entry.to_string()};
    if (new_entry == current_string){
        return;
    }
    current_string = new_entry_string;
    has_changed = true;
    return;
}

bool ScreenModelEntry::has_been_updated(bool reset){
    bool return_bool = has_changed;
    if (reset == true){
        has_changed = false;
    }
    return return_bool;
}

std::string ScreenModelEntry::get_string(){
    return current_string;
}

ScreenModelEntry &ScreenModel::get_entry(ScreenModelEntry::model_entry_types_t identifier) {

    std::map<ScreenModelEntry::model_entry_types_t,ScreenModelEntry>::iterator
             search = all_entries.find(identifier);
    if (search != all_entries.end()) {
        // ScreenModel::ScreenModelEntry< a = search;
        return search->second;
    } else {
        // ESP_LOGE("SCREENMODEL","NOT key %i",identifier);
        return empty;
    }
}

ScreenModel::ScreenModel(){
    all_entries.insert({ScreenModelEntry::ENTRY_YES,ScreenModelEntry{ScreenModelEntry::ENTRY_YES,"Yes"}});
    all_entries.insert({ScreenModelEntry::ENTRY_NO, ScreenModelEntry{ScreenModelEntry::ENTRY_NO,"No"}});                                 
    all_entries.insert({ScreenModelEntry::ENTRY_START, ScreenModelEntry{ScreenModelEntry::ENTRY_START,"Start"}});                                 
    all_entries.insert({ScreenModelEntry::ENTRY_STOP, ScreenModelEntry{ScreenModelEntry::ENTRY_STOP,"Stop"}});                                 
    
}

ScreenModelEntry::model_entry_types_t ScreenModelEntry::get_identifier() const {
    return identifier;
}

void ScreenModel::put_new_entry(ScreenModelEntry new_entry) {
    if(new_entry.get_identifier() == ScreenModelEntry::ENTRY_NON){
        return;
    }
    std::map<ScreenModelEntry::model_entry_types_t,ScreenModelEntry>::iterator
             search = all_entries.find(new_entry.get_identifier());

    if(search != all_entries.end()){
        // all_entries.insert()
        search->second.set_new(new_entry.get_string());
        return;
    }
    all_entries.insert({new_entry.get_identifier(),new_entry});
    //found_entry.set_new(new_entry.get_string());
}

void ScreenModel::put_new_entry(ScreenModelEntry::model_entry_types_t identifier, std::string value) {
    put_new_entry(ScreenModelEntry{identifier,value});
}

std::string ScreenModel::get_entry_string(ScreenModelEntry::model_entry_types_t to_get){
    ScreenModelEntry a = get_entry(to_get);
    return a.get_string();
}