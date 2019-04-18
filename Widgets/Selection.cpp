//
// Created by Kim on 25-03-2019.
//

#include "Selection.h"
#include <Gui/Widget.h>

static void OnInteractableWidgetChanged(Entity widget, const InteractableWidget& oldValue, const InteractableWidget& newValue) {
    auto selectableData = GetSelectableWidget(widget);
    if(newValue.WidgetClicked && selectableData.WidgetSelectionContext && selectableData.WidgetSelectionEntity) {
        auto contextData = GetSelectionContext(selectableData.WidgetSelectionContext);

        if(!contextData.SelectionContextMulti) {
            contextData.SelectionContextSelections.SetSize(0);
        } else {
            for(auto selection : contextData.SelectionContextSelections) {
                auto selectionData = GetSelection(selection);
                if(selectionData.SelectionEntity == selectableData.WidgetSelectionEntity) {
                    contextData.SelectionContextSelections.Remove(contextData.SelectionContextSelections.GetIndex(selection));
                    SetSelectionContext(selectableData.WidgetSelectionContext, contextData);
                    return;
                }
            }
        }

        auto selection = CreateEntity();
        auto selectionData = GetSelection(selection);
        selectionData.SelectionEntity = selectableData.WidgetSelectionEntity;
        SetSelection(selection, selectionData);

        contextData.SelectionContextSelections.Add(selection);

        SetSelectionContext(selectableData.WidgetSelectionContext, contextData);
    }
}

static void OnSelectionChanged(Entity selection, const Selection& oldValue, const Selection& newValue) {
    auto context = GetOwnership(selection).Owner;

    SelectableWidget data;
    for_entity_data(selectable, ComponentOf_SelectableWidget(), &data) {
        if(context != data.WidgetSelectionContext) continue;

        if(data.WidgetSelectionEntity == oldValue.SelectionEntity) {
            auto widgetData = GetWidget(selectable);
            widgetData.WidgetSelected = false;
            SetWidget(selectable, widgetData);
        }

        if(data.WidgetSelectionEntity == newValue.SelectionEntity) {
            auto widgetData = GetWidget(selectable);
            widgetData.WidgetSelected = true;
            SetWidget(selectable, widgetData);
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

    RegisterSystem(OnInteractableWidgetChanged, ComponentOf_InteractableWidget())
    RegisterSystem(OnSelectionChanged, ComponentOf_Selection())
EndUnit()