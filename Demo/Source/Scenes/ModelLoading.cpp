#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/ModelLoading.h>

void SceneModelLoading::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, -50.f }, 45.f, 0.001f, 1000.f);
  ModelCreate(mModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\teapot.fbx");
  ShaderCreate(mShader, sVertexShaderSourceLambert, sFragmentShaderSourceLambert);
}

void SceneModelLoading::OnDisable()
{
  ModelDestroy(mModel);
  ShaderDestroy(mShader);
}

void SceneModelLoading::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerSpace(mCamera, mCameraController, timeDelta);
}

void SceneModelLoading::OnRender() const
{
  glClearColor(0.f, 0.2f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  ShaderBind(mShader);
  ModelBind(mModel);
}