#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 1000.f);
  ModelCreate(mModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\teapot.fbx");
  BufferLayoutCreate(mBufferSteering, sObjectSize);
  RandomizeSteerings();
  BufferLayoutData(mBufferSteering, mSteerings, sObjectSize);
  ShaderCreateCompute(mShaderCompute, mShaderComputeSteering);
  ShaderCreateRender(mShaderRender, mShaderRenderVertexLambert, mShaderRenderFragmentLambert);
}
void SceneInstancing::OnDisable()
{
  ModelDestroy(mModel);
  BufferLayoutDestroy(mBufferSteering);
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
void SceneInstancing::OnRender(r32 timeDelta) const
{
  BufferLayoutBind(mBufferSteering, 0);

  //ShaderBind(mShaderCompute);
  //ShaderUniformR32(mShaderCompute, "uTimeDelta", timeDelta);
  //ShaderExecuteCompute(mShaderCompute, 16, 1, 1);

  ShaderBind(mShaderRender);
  ShaderUniformR32M4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRender, "uView", mCamera.mView);
  for (u32 i{}; i < sObjectSize; i++)
  {
    ShaderUniformU32(mShaderRender, "uObjectIndex", i);
    ModelRender(mModel);
  }
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  for (u32 i{}; i < sObjectSize; i++)
  {
    Steering const& steering{ mSteerings[i] };

    GizmoLineBatchPushLine(steering.mPosition, steering.mPosition + steering.mDirection, { 1.f, 1.f, 1.f, 1.f });
    GizmoLineBatchPushBox(steering.mPosition, { 1.f, 1.f, 1.f }, { 1.f, 0.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeSteerings()
{
  for (u32 i{}; i < sObjectSize; i++)
  {
    mSteerings[i] =
    {
      .mPosition { glm::ballRand(500.f) },
      .mDirection{ glm::sphericalRand(1.f) },
      .mAccel    {},
    };
  }
}