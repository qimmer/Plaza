//
// Created by Kim on 25-03-2019.
//

#ifndef PLAZA_SELECTION_H
#define PLAZA_SELECTION_H

#include <Core/NativeUtils.h>

struct Selection {
    Entity SelectionEntity;
};

struct SelectionContext {
    ChildArray SelectionContextSelections;
    bool SelectionContextMulti;
};

struct SelectableWidget {
    Entity WidgetSelectionContext, WidgetSelectionEntity;
};

Unit(Selection)
    Component(Selection)
        Property(Entity, SelectionEntity)

    Component(SelectionContext)
        ArrayProperty(Selection, SelectionContextSelections)
        Property(bool, SelectionContextMulti)

    Component(SelectableWidget)
        ReferenceProperty(SelectionContext, WidgetSelectionContext)
        Property(Entity, WidgetSelectionEntity)

#endif //PLAZA_SELECTION_H
