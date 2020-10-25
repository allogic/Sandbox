#pragma once

#include <Types.h>
#include <Core.h>
#include <Events.h>
#include <Common.h>
#include <Shaders.h>

#ifdef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

/*
* Debug utilities.
*/

u32 CheckShaderStatus(u32 id, u32 type, std::string& log);

/*
* OpenGL context specific.
*/

extern "C" SANDBOX_ENGINE_API void        ContextCreate(u32 width, u32 height, std::string const& title);
extern "C" SANDBOX_ENGINE_API GLFWwindow* ContextHandle();
extern "C" SANDBOX_ENGINE_API void        ContextDestroy();

/*
* Event dispatching.
*/

extern "C" SANDBOX_ENGINE_API void EventsPoll();

/*
* Mouse/Keyboard/Window state handling.
*/

extern "C" SANDBOX_ENGINE_API u32 WindowStatus();
extern "C" SANDBOX_ENGINE_API r32 WindowSizeX();
extern "C" SANDBOX_ENGINE_API r32 WindowSizeY();
extern "C" SANDBOX_ENGINE_API r32 WindowAspect();
extern "C" SANDBOX_ENGINE_API r32 MousePositionX();
extern "C" SANDBOX_ENGINE_API r32 MousePositionY();
extern "C" SANDBOX_ENGINE_API r32 MouseScrollX();
extern "C" SANDBOX_ENGINE_API r32 MouseScrollY();
extern "C" SANDBOX_ENGINE_API u32 MouseDown(u32 key);
extern "C" SANDBOX_ENGINE_API u32 MouseHeld(u32 key);
extern "C" SANDBOX_ENGINE_API u32 MouseUp(u32 key);
extern "C" SANDBOX_ENGINE_API u32 KeyDown(u32 key);
extern "C" SANDBOX_ENGINE_API u32 KeyHeld(u32 key);
extern "C" SANDBOX_ENGINE_API u32 KeyUp(u32 key);

/*
* Scene management.
*/

extern "C" SANDBOX_ENGINE_API void   SceneCreate(Scene* pScene);
extern "C" SANDBOX_ENGINE_API void   SceneSwitch(u32 index);
extern "C" SANDBOX_ENGINE_API Scene* SceneActive();
extern "C" SANDBOX_ENGINE_API void   SceneDestroyAll();

/*
* Camera management.
*/

extern "C" SANDBOX_ENGINE_API void CameraCreate(Camera& camera, r32v3 const& position, r32 fov, r32 near, r32 far);
extern "C" SANDBOX_ENGINE_API void CameraUpdateControllerInputSpace(Camera & camera, CameraControllerSpace & controller, r32 timeDelta);
extern "C" SANDBOX_ENGINE_API void CameraUpdateControllerInputOrbit(Camera & camera, CameraControllerOrbit & controller, r32 timeDelta);
extern "C" SANDBOX_ENGINE_API void CameraUpdateControllerPhysicsSpace(Camera& camera, CameraControllerSpace& controller);
extern "C" SANDBOX_ENGINE_API void CameraUpdateControllerPhysicsOrbit(Camera& camera, CameraControllerOrbit& controller);

/*
* Shader management.
*/

extern "C" SANDBOX_ENGINE_API void ShaderCreate(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void ShaderBind(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void ShaderDestroy(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void ShaderUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix);

/*
* Model management.
*/

extern "C" SANDBOX_ENGINE_API void ModelCreate(Model& model, std::string const& fileName);
extern "C" SANDBOX_ENGINE_API void ModelRender(Model const& model);
extern "C" SANDBOX_ENGINE_API void ModelDestroy(Model const& model);

/*
* 3D debug utilities.
*/

extern "C" SANDBOX_ENGINE_API void GizmoLineBatchCreate(u32 vertexBufferSize);
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchBind();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchPushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color);
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchUnbind();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchRender();