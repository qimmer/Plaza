//
// Created by Kim on 28-07-2018.
//

#include "FlowNode.h"

struct FlowNode {
    v2i FlowNodeLocation;
};

BeginUnit(FlowNode)
    BeginComponent(FlowNode)
        RegisterProperty(v2i, FlowNodeLocation)
    EndComponent()
EndUnit()