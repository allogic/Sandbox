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

extern WindowCloseClb sOnWindowClose;
extern MouseMoveClb   sOnMouseMove;
extern MouseButtonClb sOnMouseButton;
extern KeyboardClb    sOnKeyboard;

bool CheckShaderStatus(u32 id, u32 type, std::string& log);

extern "C" SANDBOX_ENGINE_API void CreateContext(Context& context, u32 width, u32 height, std::string const& title);
extern "C" SANDBOX_ENGINE_API void CreateShader(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void CreateMesh(Mesh& mesh, std::vector<r32> const& vertexLayout, u32 numVertexElements, std::vector<u32> const& indices);
extern "C" SANDBOX_ENGINE_API void CreateModel(Model& model, std::string const& fileName);
extern "C" SANDBOX_ENGINE_API void CreateCamera(Camera& camera, r32v3 const& position, r32 fov, r32 aspect, r32 near, r32 far);

extern "C" SANDBOX_ENGINE_API void UpdateCamera(Camera& camera, r32 apsect, r32 timeDelta);

extern "C" SANDBOX_ENGINE_API void ApplyUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix);

extern "C" SANDBOX_ENGINE_API void BindShader(Shader const& shader);
extern "C" SANDBOX_ENGINE_API void BindMesh(Mesh const& mesh);
extern "C" SANDBOX_ENGINE_API void BindModel(Model const& model);