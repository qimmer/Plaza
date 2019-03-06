//
// Created by Kim Johannsen on 06-03-2018.
//

#include <Foundation/FoundationModule.h>
#include <Gui/GuiModule.h>
#include <Json/NativeUtils.h>
#include <Rendering/RenderingModule.h>

#include "StbDecodersModule.h"
#include "StbImagePersistance.h"
#include "StbTrueTypePersistance.h"

BeginModule(StbDecoders)
    RegisterDependency(Gui)
    RegisterDependency(Rendering)
    RegisterUnit(StbTrueTypePersistance)
    RegisterUnit(StbImagePersistance)

    ModuleData(
            {
                "FileTypes":[
                    {
                        "Uuid": "FileType.ttf",
                        "FileTypeExtension": ".ttf",
                        "FileTypeMimeType" : "font/ttf"
                    },
                    {
                        "Uuid": "FileType.png",
                        "FileTypeExtension": ".png",
                        "FileTypeMimeType" : "image/png"
                    }
                ],
                "Children": [
                    {
                        "Uuid": "Unicode.BasicLatin",
                        "UnicodeRangeStart": 32,
                        "UnicodeRangeEnd": 128
                    },
                    {
                        "Uuid": "Unicode.Latin1",
                        "UnicodeRangeStart": 160,
                        "UnicodeRangeEnd": 256
                    },
                    {
                        "Uuid": "Unicode.LatinExtendedA",
                        "UnicodeRangeStart": 256,
                        "UnicodeRangeEnd": 383
                    },
                    {
                        "Uuid": "Unicode.LatinExtendedB",
                        "UnicodeRangeStart": 384,
                        "UnicodeRangeEnd": 591
                    }
                ]
            }
    );
EndModule()
