#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

#include <Structs/ActionEvent.h>

struct AnimationComponent
{
    Vector<ActionEvent> Actions;
    Vector<ActionEvent> ActionsReplayCache;
    ActionEvent CurrentAction;
    ActionEvent LastSerializedAction;
};
