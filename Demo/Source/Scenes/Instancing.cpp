#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 1000.f);
  ModelCreate(mModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\teapot.fbx");
  StorageLayoutCreate(mBufferVelocity, 0, sVelocitySize);
  RandomizeVelocities();
  StorageLayoutData(mBufferVelocity, mVelocities, sVelocitySize);
  ShaderCreateCompute(mShaderCompute, mShaderComputeVelocity);
  ShaderCreateRender(mShaderRender, mShaderRenderVertexLambert, mShaderRenderFragmentLambert);
}
void SceneInstancing::OnDisable()
{
  ModelDestroy(mModel);
  StorageLayoutDestroy(mBufferVelocity);
  ShaderDestroyRender(mShaderRender);
  ShaderDestroyCompute(mShaderCompute);
}
void SceneInstancing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputSpace(mCamera, mCameraController, timeDelta);
}
void SceneInstancing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsSpace(mCamera, mCameraController);
}
void SceneInstancing::OnRender() const
{
  ShaderBind(mShaderCompute);
  ShaderExecuteCompute(mShaderCompute, sVelocitySize, 1, 1);
  ShaderBind(mShaderRender);
  ShaderUniformMat4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformMat4(mShaderRender, "uView", mCamera.mView);
  ModelRender(mModel);
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  for (u32 i{}; i < sVelocitySize; i++)
  {
    Velocity const& velocity{ mVelocities[i] };

    GizmoLineBatchPushLine(velocity.mPosition, velocity.mPosition + velocity.mDirection, { 1.f, 1.f, 1.f, 1.f });
    GizmoLineBatchPushBox(velocity.mPosition, { 1.f, 1.f, 1.f }, { 1.f, 0.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeVelocities()
{
  for (u32 i{}; i < sVelocitySize; i++)
  {
    mVelocities[i] =
    {
      .mPosition { glm::ballRand(100.f) },
      .mDirection{ glm::sphericalRand(1.f) },
      .mAccel    {},
    };
  }
}