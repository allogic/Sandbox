#pragma once

#include <Types.h>
#include <Core.h>
#include <Events.h>
#include <Common.h>

#include <VertexLayouts.h>
#include <BufferLayouts.h>
#include <TextureLayouts.h>

/*
* Debug utilities.
*/

u32  CheckShaderStatus(u32 id, u32 type, std::string& log);

/*
* OpenGL context specific.
*/

extern "C" SANDBOX_ENGINE_API void        ContextCreate(u32 width, u32 height, std::string const& title);
extern "C" SANDBOX_ENGINE_API void        ContextRegisterDebugHandler();
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

extern "C" SANDBOX_ENGINE_API void ShaderCreateCompute(ShaderCompute& shaderCompute, std::string const& computeShaderSource);
extern "C" SANDBOX_ENGINE_API void ShaderCreateRender(ShaderRender& shaderRender, std::string const& renderShaderVertexcSource, std::string const& renderShaderFragmentSource);
extern "C" SANDBOX_ENGINE_API void ShaderDestroyCompute(ShaderCompute const& shaderCompute);
extern "C" SANDBOX_ENGINE_API void ShaderDestroyRender(ShaderRender const& shaderRender);
extern "C" SANDBOX_ENGINE_API void ShaderExecuteCompute(ShaderCompute const& shaderCompute, u32 numThreadsX, u32 numThreadsY, u32 numThreadsZ);

template<typename Shader> void ShaderBind(Shader const& shader)
{
  glUseProgram(shader.mPid);
}
template<typename Shader> void ShaderUniformS32(Shader const& shader, std::string const& name, s32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniform1i(uniformIndex, value);
}
template<typename Shader> void ShaderUniformU32(Shader const& shader, std::string const& name, u32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniform1ui(uniformIndex, value);
}
template<typename Shader> void ShaderUniformR32(Shader const& shader, std::string const& name, r32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniform1f(uniformIndex, value);
}
template<typename Shader> void ShaderUniformR32V3(Shader const& shader, std::string const& name, r32v3 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniform3fv(uniformIndex, 1, &value[0]);
}
template<typename Shader> void ShaderUniformR32M4(Shader const& shader, std::string const& name, r32m4 const& matrix)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, &matrix[0][0]);
}

/*
* Model management.
*/

extern "C" SANDBOX_ENGINE_API void ModelCreate(ModelLambert& model, std::string const& fileName);
extern "C" SANDBOX_ENGINE_API void ModelRender(ModelLambert const& model);
extern "C" SANDBOX_ENGINE_API void ModelRenderInstanced(ModelLambert const& model, u32 numInstances);
extern "C" SANDBOX_ENGINE_API void ModelDestroy(ModelLambert const& model);

/*
* 3D debug utilities.
*/

extern "C" SANDBOX_ENGINE_API void GizmoLineBatchCreate();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchClear();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchBind();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchUnbind();
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchPushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color);
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchPushBox(r32v3 const& center, r32v3 const& size, r32v4 const& color);
extern "C" SANDBOX_ENGINE_API void GizmoLineBatchRender();