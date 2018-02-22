//
// Created by Kim Johannsen on 27/01/2018.
//

#include "Selection.h"


    struct Selection {

    };

    DefineComponent(Selection)
    EndComponent()

    void DeselectAll() {
        while(GetNumSelection()) {
            RemoveSelection(GetSelectionEntity(0));
        }
    }
