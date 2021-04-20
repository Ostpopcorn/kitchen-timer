#include "screen_view.h"

ScreenModel* ViewBase::model{};

ViewBase::ViewBase() 
{
}

ViewBase::~ViewBase()
{
}

void ViewBase::update(bool redraw)
{
}

void ViewBase::assing_model(ScreenModel* new_model){
    ViewBase::model = new_model;
}

bool ViewBase::has_model(){
    return ViewBase::model != NULL;
}