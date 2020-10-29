#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 1000.f }, 45.f, 0.001f, 10000.f);
  ModelCreate(mModelSpaceShip, "C:\\Users\\Burmi\\Downloads\\Sandbox\\Models\\cube.fbx");
  RandomizeTransforms();
  RandomizeSteerings();
  BufferLayoutCreate(mBufferTransforms, sNumSpaceShips, 0);
  BufferLayoutCreate(mBufferSteerings, sNumSpaceShips, 1);
  BufferLayoutData(mBufferTransforms, mTransforms.data(), sNumSpaceShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), sNumSpaceShips);
  ShaderCreateCompute(mShaderCompute, mShaderComputeSpaceShipUpdate);
  ShaderCreateRender(mShaderRender, mShaderRenderSpaceShipVertex, mShaderRenderSpaceShipFragment);
}
void SceneInstancing::OnDisable()
{
  ModelDestroy(mModelSpaceShip);
  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  ShaderDestroyRender(mShaderRender);
  ShaderDestroyCompute(mShaderCompute);
}
void SceneInstancing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);

  
}
void SceneInstancing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);
}
void SceneInstancing::OnRender(r32 timeDelta) const
{
  ShaderBind(mShaderCompute);
  ShaderUniformR32(mShaderCompute, "uTimeDelta", timeDelta);
  ShaderUniformR32(mShaderCompute, "uVelocityDecay", 1.f);
  ShaderExecuteCompute(mShaderCompute, glm::ceil(sNumSpaceShips / 16), 1, 1);

  ShaderBind(mShaderRender);
  ShaderUniformR32M4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRender, "uView", mCamera.mView);
  ModelRenderInstanced(mModelSpaceShip, sNumSpaceShips);
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  return;

  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    Transform const& transform{ mTransforms[i] };
    Steering const& steering{ mSteerings[i] };

    //GizmoLineBatchPushBox(transform.mPosition, transform.mScale, { 1.f, 0.f, 1.f, 1.f });
    //GizmoLineBatchPushLine(transform.mPosition, transform.mPosition + steering.mVelocity, { 1.f, 1.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeTransforms()
{
  mTransforms.resize(sNumSpaceShips);

  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    r32v3 position{ glm::ballRand(500.f) };

    mTransforms[i] =
    {
      .mPosition{ position },
    };
  }
}
void SceneInstancing::RandomizeSteerings()
{
  mSteerings.resize(sNumSpaceShips);

  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    mSteerings[i] =
    {
      .mAcceleration{ glm::sphericalRand(0.001f) },
      .mVelocity    { 0.f, 0.f, 0.f },
    };
  }
}