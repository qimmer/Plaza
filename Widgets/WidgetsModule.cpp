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
                   "TrueTypeFontSize": 14.0,
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
                       "Uuid": "Style.Default.StyleSheet.MenuItem.Enabled",
                       "SubTexture2DOffset": [
                       0,
                       27
                       ],
                       "SubTexture2DSize": [
                       5,
                       5
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.MenuItem.Selected",
                       "SubTexture2DOffset": [
                       7,
                       14
                       ],
                       "SubTexture2DSize": [
                       4,
                       4
                       ]
                   },
                   {
                       "Uuid": "Style.Default.StyleSheet.MenuItem.Clicked",
                       "SubTexture2DOffset": [
                       7,
                       7
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
                       0,
                       7
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
                       6,
                       6
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
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshDisabledTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshSelectedTexture": "Style.Default.StyleSheet.Button.Enabled",

                   "WidgetMeshHoveredTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshFocusedTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshClickedTexture": "Style.Default.StyleSheet.Button.Clicked",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "WellStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Well",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "ButtonStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshDisabledTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshSelectedTexture": "Style.Default.StyleSheet.Button.Enabled",

                   "WidgetMeshHoveredTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshFocusedTexture": "Style.Default.StyleSheet.Button.Enabled",
                   "WidgetMeshClickedTexture": "Style.Default.StyleSheet.Button.Clicked",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "MenuItemStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshDisabledTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshSelectedTexture": "Style.Default.StyleSheet.MenuItem.Selected",

                   "WidgetMeshHoveredTexture": "Style.Default.StyleSheet.MenuItem.Selected",
                   "WidgetMeshFocusedTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshClickedTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshFixedBorderWidth": 3
               },

               "MainMenuItemStyleMesh": {
                   "WidgetMeshEnabledTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshDisabledTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshSelectedTexture": "Style.Default.StyleSheet.MenuItem.Clicked",

                   "WidgetMeshHoveredTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshFocusedTexture": "Style.Default.StyleSheet.MenuItem.Enabled",
                   "WidgetMeshClickedTexture": "Style.Default.StyleSheet.MenuItem.Clicked",
                   "WidgetMeshFixedBorderWidth": 3
               },
               "ButtonStylePadding": [5, 5, 5, 5],
               "ButtonStyleFont": "Font.DroidSans",

               "MainMenuStylePadding": [3, 3, 3, 3],
               "MenuItemStylePadding": [7, 2, 7, 2],

               "PanelStylePadding": [5, 5, 5, 5]
           }
        ]
    })

    RegisterUnit(MainFrame)
EndModule()