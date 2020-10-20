#pragma once

#include <Types.h>
#include <Core.h>
#include <Common.h>

#ifdef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

extern s32 sStatus;
extern u32 sWidth;
extern u32 sHeight;
extern r32 sAspect;

extern WindowCloseClb sOnWindowClose;
extern MouseMoveClb   sOnMouseMove;
extern MouseButtonClb sOnMouseButton;
extern KeyboardClb    sOnKeyboard;

/*
* Debug utilities.
*/

u32 CheckShaderStatus(u32 id, u32 type, std::string& log);

/*
* OpenGL context specific.
*/

extern "C" SANDBOX_ENGINE_API void ContextCreate(GLFWwindow*& pHandle, u32 width, u32 height, std::string const& title);

/*
* Scene management.
*/

extern "C" SANDBOX_ENGINE_API void SceneCreate(Scene* pScene);
extern "C" SANDBOX_ENGINE_API void SceneSwitch(u32 index);
extern "C" SANDBOX_ENGINE_API void SceneActive(Scene*& pScene);
extern "C" SANDBOX_ENGINE_API void SceneDestroyAll();

/*
* Camera management.
*/

extern "C" SANDBOX_ENGINE_API void CameraCreate(Camera& camera, r32v3 const& position, r32 fov, r32 aspect, r32 near, r32 far);
extern "C" SANDBOX_ENGINE_API void CameraUpdateController(Camera& camera, CameraControllerOrbit& controller, r32 timeDelta);

/*
* Shader management.
*/

extern "C" SANDBOX_ENGINE_API void ShaderCreate(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void ShaderDestroy(Shader const& shader);

/*
* Geometry management.
*/

extern "C" SANDBOX_ENGINE_API void MeshCreate(Mesh& mesh, std::vector<r32> const& vertexLayout, u32 numVertexElements, std::vector<u32> const& indices);
extern "C" SANDBOX_ENGINE_API void MeshDestroy(Mesh const& mesh);
extern "C" SANDBOX_ENGINE_API void ModelCreate(Model& model, std::string const& fileName);
extern "C" SANDBOX_ENGINE_API void ModelDestroy(Model const& model);

extern "C" SANDBOX_ENGINE_API void ApplyUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix);

extern "C" SANDBOX_ENGINE_API void BindShader(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void BindMesh(Mesh const& mesh);
extern "C" SANDBOX_ENGINE_API void BindModel(Model const& model);

extern "C" SANDBOX_ENGINE_API void LineBatchBegin();
extern "C" SANDBOX_ENGINE_API void LinePush(r32v3 const& p0, r32v3 const& p1, r32v3 const& c0, r32v3 const& c1);
extern "C" SANDBOX_ENGINE_API void LineBatchEnd();
extern "C" SANDBOX_ENGINE_API void LineBatchRender(Shader const& shader);