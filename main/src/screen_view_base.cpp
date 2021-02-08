#include "screen_view.h"

ScreenModel<model_entry_types_t,std::string>* ViewBase::model{};

ViewBase::ViewBase() 
{
}

ViewBase::~ViewBase()
{
}

void ViewBase::update()
{
}

void ViewBase::assing_model(ScreenModel<model_entry_types_t,std::string>* new_model){
    ViewBase::model = new_model;
}

bool ViewBase::has_model(){
    return ViewBase::model != NULL;
}