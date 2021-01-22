#include "screen_view.h"

ScreenModel* ViewBase::model{NULL};

ViewBase::ViewBase() 
{
}

ViewBase::~ViewBase()
{
}

void ViewBase::update()
{
}

void ViewBase::assing_model(ScreenModel* new_model){
    ViewBase::model = new_model;
}

bool ViewBase::has_model(){
    return ViewBase::model != NULL;
}