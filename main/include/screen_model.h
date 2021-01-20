#ifndef TIMER_SCREENMODEL_H
#define TIMER_SCREENMODEL_H

#include <string>
#include <map>


class ScreenModelEntry{
public:
    typedef enum{
        ENTRY_NON=0,
        ENTRY_YES =1,
        ENTRY_NO,
        ENTRY_STARTUP = 10,
        ENTRY_PRIMARY_TIMER = 100,
    } model_entry_types_t;
    ScreenModelEntry();
    ScreenModelEntry(model_entry_types_t identifier);
    ScreenModelEntry(model_entry_types_t identifier, std::string value);

    bool operator<(ScreenModelEntry const &rhs) const;

    bool operator!=(ScreenModelEntry const &rhs) const ;
    template<class T>
    void set_new(T new_entry);

    bool has_been_updated(bool reset = true);

    std::string get_string();

    model_entry_types_t get_identifier() const;

private:
    bool has_changed{false};
    model_entry_types_t identifier{ENTRY_NON};
    std::string current_string{""};

};

class  ScreenModel{
public:
    
    
    ScreenModel();
    void put_new_entry(ScreenModelEntry);
    void put_new_entry(ScreenModelEntry::model_entry_types_t identifier, std::string value);
    std::string get_entry_string(ScreenModelEntry::model_entry_types_t);
private:
    ScreenModelEntry &get_entry(ScreenModelEntry::model_entry_types_t);
    std::map<ScreenModelEntry::model_entry_types_t,ScreenModelEntry> all_entries{};
public:
    ScreenModelEntry empty{};
};



#endif