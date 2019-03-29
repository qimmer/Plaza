//
// Created by Kim on 25-03-2019.
//

#include "Selection.h"
#include <Gui/Widget.h>

LocalFunction(OnWidgetClickedChanged, void, Entity widget, bool oldValue, bool newValue) {
    auto selectableData = GetSelectableWidgetData(widget);
    if(newValue && selectableData && selectableData->WidgetSelectionContext && selectableData->WidgetSelectionEntity) {
        if(!GetSelectionContextMulti(selectableData->WidgetSelectionContext)) {
            SetNumSelectionContextSelections(selectableData->WidgetSelectionContext, 0);
        } else {
            for_children(selection, SelectionContextSelections, selectableData->WidgetSelectionContext) {
                if(GetSelectionEntity(selection) == selectableData->WidgetSelectionEntity) {
                    RemoveSelectionContextSelectionsByValue(selectableData->WidgetSelectionContext, selection);
                    return;
                }
            }
        }

        auto selection = AddSelectionContextSelections(selectableData->WidgetSelectionContext);
        SetSelectionEntity(selection, selectableData->WidgetSelectionEntity);
    }
}

LocalFunction(OnSelectionEntityChanged, void, Entity selection, Entity oldValue, Entity newValue) {
    auto context = GetOwnership(selection).Owner;

    for_entity(selectable, ComponentOf_SelectableWidget()) {
        if(data->WidgetSelectionContext != data->WidgetSelectionContext) continue;

        if(data->WidgetSelectionEntity == oldValue) {
            SetWidgetSelected(selectable, false);
        }

        if(data->WidgetSelectionEntity == newValue) {
            SetWidgetSelected(selectable, true);
        }
    }
}

BeginUnit(Selection)
    BeginComponent(Selection)
        RegisterProperty(Entity, SelectionEntity)
    EndComponent()
    BeginComponent(SelectionContext)
        RegisterArrayProperty(Selection, SelectionContextSelections)
        RegisterProperty(bool, SelectionContextMulti)
    EndComponent()
    BeginComponent(SelectableWidget)
        RegisterReferenceProperty(SelectionContext, WidgetSelectionContext)
        RegisterProperty(Entity, WidgetSelectionEntity)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_WidgetClicked()), OnWidgetClickedChanged, 0)
EndUnit()