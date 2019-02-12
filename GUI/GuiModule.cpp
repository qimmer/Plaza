//
// Created by Kim on 19-10-2018.
//

#include <Json/NativeUtils.h>
#include <Foundation/FoundationModule.h>
#include <Rendering/RenderingModule.h>
#include <Scene/SceneModule.h>

#include "GuiModule.h"
#include "Widget.h"
#include "TextWidget.h"
#include "Font.h"
#include "GuiPickRay.h"
#include "DragHandle.h"
#include "Layout.h"

BeginModule(Gui)
    RegisterDependency(Foundation)
    RegisterDependency(Rendering)
    RegisterDependency(Scene)

    ModuleData({
       "Children": [
       {
           "Uuid": "Gui.RenderPath",
           "RenderPathPasses": [
           {
               "RenderPassClearDepth": 1.0,
               "RenderPassClearColor": [64, 64, 64, 255],
               "RenderPassClearTargets": "ClearTarget_Depth|ClearTarget_Color",
               "RenderPassMeshUniforms": [
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshEnabledTexture",
                   "UniformIdentifier": "s_widgetMeshEnabledTexture",
                   "UniformSamplerIndex": 0,
                   "UniformEntityProperty": "Property.WidgetMeshEnabledTexture"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshDisabledTexture",
                   "UniformIdentifier": "s_widgetMeshDisabledTexture",
                   "UniformSamplerIndex": 1,
                   "UniformEntityProperty": "Property.WidgetMeshDisabledTexture"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshSelectedTexture",
                   "UniformIdentifier": "s_widgetMeshSelectedTexture",
                   "UniformSamplerIndex": 2,
                   "UniformEntityProperty": "Property.WidgetMeshSelectedTexture"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshHoveredTexture",
                   "UniformIdentifier": "s_widgetMeshHoveredTexture",
                   "UniformSamplerIndex": 3,
                   "UniformEntityProperty": "Property.WidgetMeshHoveredTexture"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshFocusedTexture",
                   "UniformIdentifier": "s_widgetMeshFocusedTexture",
                   "UniformSamplerIndex": 4,
                   "UniformEntityProperty": "Property.WidgetMeshFocusedTexture"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.WidgetMeshClickedTexture",
                   "UniformIdentifier": "s_widgetMeshClickedTexture",
                   "UniformSamplerIndex": 5,
                   "UniformEntityProperty": "Property.WidgetMeshClickedTexture"
               }
               ],
               "RenderPassRenderableUniforms": [
               {
                   "Uuid": "Gui.RenderPath.Uniforms.TextWidgetTexture",
                   "UniformIdentifier": "s_fontAtlas",
                   "UniformSamplerIndex": 0,
                   "UniformEntityProperty": "Property.TextWidgetFont"
               },
               {
                   "Uuid": "Gui.RenderPath.Uniforms.Size2D",
                   "UniformIdentifier": "u_widgetSize",
                   "UniformEntityProperty": "Property.Size2D"
               }
               ],
               "RenderPassRenderState": {
                   "RenderStateBlendMode": "RenderState_STATE_BLEND_ALPHA",
                   "RenderStateDepthTest": "RenderState_STATE_DEPTH_TEST_NONE"
               },
               "RenderPassSortMode": "RenderingSortMode_DepthAscending"
           }
           ]
       },
       {
           "Uuid": "Gui.Material",
           "MaterialProgram": "Gui.Program"
       },
       {
           "Uuid": "Gui.Program",
           "Children": [
           {
               "Uuid": "Gui.Program.VertexShader",
               "StreamPath": "file://Gui/GuiProgram.vs"
           },
           {
               "Uuid": "Gui.Program.PixelShader",
               "StreamPath": "file://Gui/GuiProgram.ps"
           },
           {
               "Uuid": "Gui.Program.DeclShader",
               "StreamPath": "file://Gui/GuiProgram.var"
           }
           ],
           "ProgramVertexShaderSource": "Gui.Program.VertexShader",
           "ProgramPixelShaderSource": "Gui.Program.PixelShader",
           "ProgramDeclShaderSource": "Gui.Program.DeclShader"
       },
       {
           "Uuid": "Gui.VertexDeclaration",
           "VertexDeclarationAttributes": [
           {
               "Uuid": "Gui.VertexDeclaration.Position",
               "VertexAttributeType": "v3f",
               "VertexAttributeUsage": "VertexAttributeUsage_Position"
           },
           {
               "Uuid": "Gui.VertexDeclaration.Normal",
               "VertexAttributeType": "v3f",
               "VertexAttributeUsage": "VertexAttributeUsage_Normal"
           },
           {
               "Uuid": "Gui.VertexDeclaration.TexCoord0",
               "VertexAttributeType": "v2f",
               "VertexAttributeUsage": "VertexAttributeUsage_TexCoord0"
           },
           {
               "Uuid": "Gui.VertexDeclaration.Alignment",
               "VertexAttributeType": "v2f",
               "VertexAttributeUsage": "VertexAttributeUsage_TexCoord1"
           }
           ]
       },
       {
           "Uuid": "Gui.Font.Material",
           "MaterialProgram": "Gui.Font.Program"
       },
       {
           "Uuid": "Gui.Font.Program",
           "Children": [
           {
               "Uuid": "Gui.Font.Program.VertexShader",
               "StreamPath": "file://Gui/FontProgram.vs"
           },
           {
               "Uuid": "Gui.Font.Program.PixelShader",
               "StreamPath": "file://Gui/FontProgram.ps"
           },
           {
               "Uuid": "Gui.Font.Program.DeclShader",
               "StreamPath": "file://Gui/FontProgram.var"
           }
           ],
           "ProgramVertexShaderSource": "Gui.Font.Program.VertexShader",
           "ProgramPixelShaderSource": "Gui.Font.Program.PixelShader",
           "ProgramDeclShaderSource": "Gui.Font.Program.DeclShader"
       },
       {
           "Uuid": "Gui.Font.VertexDeclaration",
           "VertexDeclarationAttributes": [
           {
               "Uuid": "Gui.Font.VertexDeclaration.Position",
               "VertexAttributeType": "v2f",
               "VertexAttributeUsage": "VertexAttributeUsage_Position"
           },
           {
               "Uuid": "Gui.Font.VertexDeclaration.TexCoord0",
               "VertexAttributeType": "v2f",
               "VertexAttributeUsage": "VertexAttributeUsage_TexCoord0"
           }
           ]
       },
       {
           "Uuid": "Gui.Meshes",
           "Children":[
           {
               "Uuid": "Gui.Meshes.Rect",
               "MeshSubMeshes":[
               {
                   "Uuid": "Gui.Meshes.Rect.SubMesh0",
                   "SubMeshNumVertices": 4,
                   "SubMeshNumIndices": 6,
                   "SubMeshPrimitiveType": "PrimitiveType_TRIANGLELIST"
               }
               ],
               "MeshVertexBuffer": {
                   "VertexBufferDeclaration": "Gui.VertexDeclaration"
               },
               "MeshBuilderVertices":[
               {
                   "MeshBuilderVertexPosition":[0.0, 0.0, 0.0],
                   "MeshBuilderVertexNormal":[0.0, 0.0, -1.0],
                   "MeshBuilderVertexTexCoord0":[0.0, 0.0],
                   "MeshBuilderVertexTexCoord1":[0.0, 0.0]
               },
               {
                   "MeshBuilderVertexPosition":[0.0, 0.0, 0.0],
                   "MeshBuilderVertexNormal":[0.0, 0.0, -1.0],
                   "MeshBuilderVertexTexCoord0":[1.0, 0.0],
                   "MeshBuilderVertexTexCoord1":[1.0, 0.0]
               },
               {
                   "MeshBuilderVertexPosition":[0.0, 0.0, 0.0],
                   "MeshBuilderVertexNormal":[0.0, 0.0, -1.0],
                   "MeshBuilderVertexTexCoord0":[0.0, 1.0],
                   "MeshBuilderVertexTexCoord1":[0.0, 1.0]
               },
               {
                   "MeshBuilderVertexPosition":[0.0, 0.0, 0.0],
                   "MeshBuilderVertexNormal":[0.0, 0.0, -1.0],
                   "MeshBuilderVertexTexCoord0":[1.0, 1.0],
                   "MeshBuilderVertexTexCoord1":[1.0, 1.0]
               }
               ],
               "MeshBuilderIndices":[
               { "MeshBuilderIndexVertexIndex": 0 },
               { "MeshBuilderIndexVertexIndex": 1 },
               { "MeshBuilderIndexVertexIndex": 2 },
               { "MeshBuilderIndexVertexIndex": 2 },
               { "MeshBuilderIndexVertexIndex": 1 },
               { "MeshBuilderIndexVertexIndex": 3 }
               ]
           }
           ]
       }
       ]
   })

    RegisterUnit(Widget)
    RegisterUnit(Layout)
    RegisterUnit(Font)
    RegisterUnit(TextWidget)
    RegisterUnit(GuiPickRay)
    RegisterUnit(DragHandle)
EndModule()
