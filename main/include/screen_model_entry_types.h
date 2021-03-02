#if !defined(SCREEN_MODEL_ENTRY_TYPES_H)
#define SCREEN_MODEL_ENTRY_TYPES_H


typedef enum{
    ENTRY_NON=0,
    
    ENTRY_STARTUP = 2,

    ENTRY_YES =10,
    ENTRY_NO,
    ENTRY_START,
    ENTRY_STOP,
    ENTRY_PRIMARY_TIMER = 100,
    ENTRY_BATTERY_VOLTAGE = 200,
} model_entry_types_t;

#endif // SCREEN_MODEL_ENTRY_TYPES_H