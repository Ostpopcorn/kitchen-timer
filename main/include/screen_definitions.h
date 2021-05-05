#ifndef SCREEN_DEFINITIONS_H
#define SCREEN_DEFINITIONS_H

#include "DataModel.h"
#include "screen_model_entry_types.h"
#include "battery_display.h"
#include "counter_clock.h"

using DataModelType = DataModel<model_entry_types_t,int,std::string,BatteryDisplay,CounterClock> ;

#endif