//
// Created by Kim on 17-01-2019.
//

#include "WidgetsModule.h"
#include <Gui/GuiModule.h>
#include <Json/NativeUtils.h>

#include "Button.h"
#include "Checkbox.h"
#include "Dock.h"
#include "MainFrame.h"
#include "Panel.h"
#include "Style.h"
#include "Menu.h"

BeginModule(Widgets)
    RegisterDependency(Gui)

    RegisterUnit(Style)
    RegisterUnit(Button)
    //RegisterUnit(Checkbox)
    RegisterUnit(Dock)
    RegisterUnit(Panel)
    RegisterUnit(Menu)

    ModuleData({
        "Children": [
           {
               "Uuid": "Style.Default",

               "StyleFonts": [
               {
                   "Uuid": "Font.DroidSans",
                   "TrueTypeFontSize": 10.0,
                   "FontCharacters": "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+-!\"@#$%&/()=?'*\\{[]}.,:;<>| ",
                   "StreamPath": "file://Widgets/Fonts/DroidSans.ttf",
                   "PersistancePointLoaded": true
               }
               ],

               "StyleSheet": {
                   "StreamPath": "file://Widgets/Styles/Default.png",
                   "TextureFlag": "TextureFlag_MIN_POINT|TextureFlag_MAG_POINT",
                   "PersistancePointLoaded": true,
                   "TextureAtlasSubTextures": [
                   {
                       "Uuid": "Style.Default.StyleSheet.Dock",
                       "SubTexture2DOffset": [
                       0,
                       0
                       ],
                       "SubTexture2DSize": [
                       6,
                       6
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.Panel",
                       "SubTexture2DOffset": [
                       0,
                       0
                       ],
                       "SubTexture2DSize": [
                       6,
                       6
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.Well",
                       "SubTexture2DOffset": [
                       7,
                       7
                       ],
                       "SubTexture2DSize": [
                       5,
                       5
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.Button.Enabled",
                       "SubTexture2DOffset": [
                       8,
                       0
                       ],
                       "SubTexture2DSize": [
                       5,
                       5
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.Button.Clicked",
                       "SubTexture2DOffset": [
                       7,
                       7
                       ],
                       "SubTexture2DSize": [
                       5,
                       5
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.Button.Focused",
                       "SubTexture2DOffset": [
                       0,
                       19
                       ],
                       "SubTexture2DSize": [
                       7,
                       7
                       ]
                   }
                   ]
               },

               "DockStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Dock",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "PanelStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Panel",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "WellStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Well",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "ButtonStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshFocusedTexture": "Style.Default.StyleSheet.Button.Focused",
                   "WidgetMeshClickedTexture": "Style.Default.StyleSheet.Button.Clicked",
                   "WidgetMeshFixedBorderWidth": 3
               },
               "ButtonStylePadding": [5, 5, 5, 5],
               "ButtonStyleFont": "Font.DroidSans",

               "PanelStylePadding": [0, 5, 0, 5]
           }
        ]
    })

    RegisterUnit(MainFrame)
EndModule()