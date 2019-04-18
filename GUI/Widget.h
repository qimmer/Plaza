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
    Entity WidgetMesh;
    bool WidgetDisabled, WidgetSelected;
    float WidgetDepthOrder;
};

struct InteractableWidget {
    v2i WidgetInteractionPoint;
    bool WidgetHovered, WidgetFocused, WidgetClicked;
};

struct WidgetState {
    v3f WidgetState;
    v3f WidgetInteractionState;
    rgba32 WidgetStateColor;
    float WidgetStateTransitionDuration;
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
        Property(bool, WidgetDisabled)
        Property(bool, WidgetSelected)
        Property(float, WidgetDepthOrder)

    Component(InteractableWidget)
        Property(v2i, WidgetInteractionPoint)
        Property(bool, WidgetHovered)
        Property(bool, WidgetFocused)
        Property(bool, WidgetClicked)

    Component(WidgetState)
        Property(rgba32, WidgetStateColor)
        Property(float, WidgetStateTransitionDuration)
        Property(v3f, WidgetState)
        Property(v3f, WidgetInteractionState)

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
