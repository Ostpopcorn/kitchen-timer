#include "screen_view.h"
#include "screen_definitions.h"

DataModelType* ViewBase::model{};

ViewBase::ViewBase() 
{
}

ViewBase::~ViewBase()
{
}

void ViewBase::update(bool redraw)
{
}

void ViewBase::assing_model(DataModelType* new_model){
    ViewBase::model = new_model;
}

bool ViewBase::has_model(){
    return ViewBase::model != NULL;
}