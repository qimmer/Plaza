//
// Created by Kim Johannsen on 03-04-2018.
//

#ifndef PLAZA_WIDGET_H
#define PLAZA_WIDGET_H

#include <Core/NativeUtils.h>

struct Rect2D {
    v2i Size2D;
};

struct Widget {
    rgba32 WidgetStateColor;
    Entity WidgetModel, WidgetMesh;
    bool WidgetDisabled, WidgetSelected;
    float WidgetStateTransitionDuration;
    v3f WidgetState;
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
    rgba32 WidgetMeshEnabledColor;
    rgba32 WidgetMeshDisabledColor;
    rgba32 WidgetMeshSelectedColor;

    Entity WidgetMeshEnabledTexture;
    Entity WidgetMeshDisabledTexture;
    Entity WidgetMeshSelectedTexture;

    u16 WidgetMeshFixedBorderWidth;
};

struct InteractableWidgetMesh {
    rgba32 WidgetMeshHoveredColor;
    rgba32 WidgetMeshFocusedColor;
    rgba32 WidgetMeshClickedColor;

    Entity WidgetMeshHoveredTexture;
    Entity WidgetMeshFocusedTexture;
    Entity WidgetMeshClickedTexture;
};

Unit(Widget)
    Enum(WidgetChildLayout)
    Enum(WidgetState)

    Component(Rect2D)
        Property(v2i, Size2D)

    Component(Widget)
        Property(Entity, WidgetModel)
        Property(float, WidgetStateTransitionDuration)
        Property(v3f, WidgetState)
        Property(bool, WidgetDisabled)
        Property(bool, WidgetSelected)
        Property(rgba32, WidgetStateColor)

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
        Property(rgba32, WidgetMeshEnabledColor)
        Property(rgba32, WidgetMeshDisabledColor)
        Property(rgba32, WidgetMeshSelectedColor)
        Property(u16, WidgetMeshFixedBorderWidth)

    Component(InteractableWidgetMesh)
        ReferenceProperty(SubTexture2D, WidgetMeshHoveredTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshFocusedTexture)
        ReferenceProperty(SubTexture2D, WidgetMeshClickedTexture)
        Property(rgba32, WidgetMeshHoveredColor)
        Property(rgba32, WidgetMeshFocusedColor)
        Property(rgba32, WidgetMeshClickedColor)


#endif //PLAZA_WIDGET_H
