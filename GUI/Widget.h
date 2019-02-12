//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/NativeUtils.h>

struct Rect2D {
    v2f Size2D;
};

struct Widget {
    Entity WidgetModel, WidgetMesh;
    bool WidgetDisabled, WidgetSelected;
    float WidgetStateTransitionDuration;
    v2f WidgetState;
};

struct InteractableWidget {
    v2i WidgetInteractionPoint;
    bool WidgetHovered, WidgetFocused, WidgetClicked;
    v3f WidgetInteractionState;
};

struct WidgetVertex {
    v3f Position, Normal;
    v2f TexCoord, Alignment;
};

struct WidgetMesh {
    Entity WidgetMeshEnabledTexture;
    Entity WidgetMeshDisabledTexture;
    Entity WidgetMeshSelectedTexture;

    u16 WidgetMeshFixedBorderWidth;
};

struct InteractableWidgetMesh {
    Entity WidgetMeshHoveredTexture;
    Entity WidgetMeshFocusedTexture;
    Entity WidgetMeshClickedTexture;
};

Unit(Widget)
    Enum(WidgetChildLayout)
    Enum(WidgetState)

    Component(Rect2D)
        Property(v2f, Size2D)

    Component(Widget)
        Property(Entity, WidgetModel)
        Property(float, WidgetStateTransitionDuration)
        Property(v2f, WidgetState)
        Property(bool, WidgetDisabled)
        Property(bool, WidgetSelected)

    Component(InteractableWidget)
        Property(v2i, WidgetInteractionPoint)
        Property(v3f, WidgetInteractionState)
        Property(bool, WidgetHovered)
        Property(bool, WidgetFocused)
        Property(bool, WidgetClicked)

    Component(WidgetMesh)
        ReferenceProperty(SubTexture2D, WidgetMeshEnabledTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshDisabledTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshSelectedTexture)
        Property(u16, WidgetMeshFixedBorderWidth)

    Component(InteractableWidgetMesh)
        ReferenceProperty(SubTexture2D, WidgetMeshHoveredTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshFocusedTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshClickedTexture)


#endif //PLAZA_WIDGET_H
