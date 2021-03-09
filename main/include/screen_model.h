#ifndef TIMER_SCREENMODEL_H
#define TIMER_SCREENMODEL_H

#include "string.h"
#include <map>


template<class identifier_t,class object_type_t >
class ScreenModelEntry{
public:
    ScreenModelEntry();
    ScreenModelEntry(identifier_t identifier);
    ScreenModelEntry(identifier_t identifier, object_type_t value);

    bool operator<(ScreenModelEntry const &rhs) const;

    bool operator!=(ScreenModelEntry const &rhs) const;

    void set_new(object_type_t new_entry);

    bool has_been_updated(bool reset = true);

    object_type_t get_object();

    identifier_t get_identifier() const;

private:
    bool has_changed{false};
    identifier_t identifier{};
    object_type_t current_object{};

};

template<class identifier_t,class object_type_t >
class  ScreenModel{
public:
    
    ScreenModel();
    void put_new_entry(ScreenModelEntry<identifier_t,object_type_t> new_entry);

    void put_new_entry(identifier_t identifier, object_type_t value);

    ScreenModelEntry<identifier_t,object_type_t>& get_entry(identifier_t identifier);
    object_type_t get_entry_object(identifier_t identifier);
    std::string get_entry_string(identifier_t identifier);
private:

    std::map<identifier_t,ScreenModelEntry<identifier_t,object_type_t>> all_entries{};
    ScreenModelEntry<identifier_t,object_type_t> empty{};
};

#include "screen_model.tpp"

#endif