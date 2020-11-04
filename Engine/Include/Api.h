#pragma once

#include <Types.h>
#include <Core.h>
#include <Events.h>
#include <Camera.h>
#include <Scene.h>

#include <VertexLayouts.h>
#include <BufferLayouts.h>
#include <TextureLayouts.h>
#include <ShaderLayouts.h>
#include <UniformLayouts.h>

#include <FrameBuffer.h>
#include <DepthBuffer.h>
#include <GraphicBuffer.h>

/*
* OpenGL context specific.
*/

extern "C" void        ContextCreate(u32 width, u32 height, std::string const& title);
extern "C" void        ContextRegisterDebugHandler();
extern "C" GLFWwindow* ContextHandle();
extern "C" void        ContextDestroy();

/*
* Event dispatching.
*/

extern "C" void EventsPoll();

/*
* Mouse/Keyboard/Window state handling.
*/

extern "C" u32 WindowStatus();
extern "C" r32 WindowSizeX();
extern "C" r32 WindowSizeY();
extern "C" r32 WindowAspect();
extern "C" r32 MousePositionX();
extern "C" r32 MousePositionY();
extern "C" r32 MouseScrollX();
extern "C" r32 MouseScrollY();
extern "C" u32 MouseDown(u32 key);
extern "C" u32 MouseHeld(u32 key);
extern "C" u32 MouseUp(u32 key);
extern "C" u32 KeyDown(u32 key);
extern "C" u32 KeyHeld(u32 key);
extern "C" u32 KeyUp(u32 key);

/*
* Scene management.
*/

extern "C" void   SceneCreate(Scene* pScene);
extern "C" void   SceneSwitch(u32 index);
extern "C" Scene* SceneActive();
extern "C" void   SceneDestroyAll();

/*
* Model management.
*/

extern "C" void ModelCreate(ModelLambert& model, std::string const& fileName);
extern "C" void ModelRender(ModelLambert const& model);
extern "C" void ModelRenderInstanced(ModelLambert const& model, u32 numInstances);
extern "C" void ModelDestroy(ModelLambert const& model);

/*
* 3D render passes.
*/



/*
* 3D debug utilities.
*/

extern "C" void GizmoLineBatchCreate();
extern "C" void GizmoLineBatchClear();
extern "C" void GizmoLineBatchBind();
extern "C" void GizmoLineBatchPushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color);
extern "C" void GizmoLineBatchPushBox(r32v3 const& center, r32v3 const& size, r32v4 const& color);
extern "C" void GizmoLineBatchRender();