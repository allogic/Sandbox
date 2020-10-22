#pragma once

#include <Types.h>
#include <Core.h>
#include <Common.h>
#include <Shaders.h>

#ifdef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <glad/glad.h>

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

extern "C" SANDBOX_ENGINE_API void EventStateNext();

/*
* Mouse/Keyboard/Window state handling.
*/

extern "C" SANDBOX_ENGINE_API u32 WindowStatus();
extern "C" SANDBOX_ENGINE_API r32 WindowSizeX();
extern "C" SANDBOX_ENGINE_API r32 WindowSizeY();
extern "C" SANDBOX_ENGINE_API r32 WindowAspect();
extern "C" SANDBOX_ENGINE_API r32 MousePositionX();
extern "C" SANDBOX_ENGINE_API r32 MousePositionY();
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
extern "C" SANDBOX_ENGINE_API void CameraUpdateController(Camera& camera, CameraControllerOrbit& controller, r32 timeDelta);

/*
* Shader management.
*/

extern "C" SANDBOX_ENGINE_API void ShaderCreate(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void ShaderBind(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void ShaderDestroy(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void ShaderUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix);

/*
* Geometry management.
*/

extern "C" SANDBOX_ENGINE_API void MeshCreate(Mesh& mesh, std::vector<Vertex> const& vertices, std::vector<u32> const& indices);
extern "C" SANDBOX_ENGINE_API void MeshBind(Mesh const& mesh);
extern "C" SANDBOX_ENGINE_API void MeshDestroy(Mesh const& mesh);
extern "C" SANDBOX_ENGINE_API void ModelCreate(Model& model, std::string const& fileName);
extern "C" SANDBOX_ENGINE_API void ModelBind(Model const& model);
extern "C" SANDBOX_ENGINE_API void ModelDestroy(Model const& model);

/*
* 3D debug utilities.
*/

extern "C" SANDBOX_ENGINE_API void LineBatchCreate();
extern "C" SANDBOX_ENGINE_API void LineBatchBegin();
extern "C" SANDBOX_ENGINE_API void LineBatchPush(r32v3 const& p0, r32v3 const& p1, r32v4 const& c0, r32v4 const& c1);
extern "C" SANDBOX_ENGINE_API void LineBatchEnd();
extern "C" SANDBOX_ENGINE_API void LineBatchRender();
extern "C" SANDBOX_ENGINE_API void LineBatchDestroy();