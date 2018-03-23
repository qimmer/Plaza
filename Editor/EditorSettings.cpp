//
// Created by Kim Johannsen on 21-03-2018.
//

#include "EditorSettings.h"

static void Start() {

}

static void Stop() {

}

DefineService(EditorSettings)
    Subscribe(EditorSettingsStarted, Start)
    Subscribe(EditorSettingsStopped, Stop)
EndService()