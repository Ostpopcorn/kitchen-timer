#include "screen_view.h"


ViewBase::ViewBase() 
{
}

ViewBase::ViewBase(ScreenModel & model) : model{model}
{
}

ViewBase::~ViewBase()
{
}
