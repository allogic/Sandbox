#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 10000.f);
  ModelCreate(mModelShip, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\cube.fbx");

  RandomizeTransforms();
  RandomizeSteerings();
  RandomizeWaypoints();

  BufferLayoutCreate(mBufferTransforms, mNumShips, 0);
  BufferLayoutCreate(mBufferSteerings, mNumShips, 1);
  BufferLayoutCreate(mBufferWaypoints, mNumWaypoints, 2);

  BufferLayoutData(mBufferTransforms, mTransforms.data(), mNumShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), mNumShips);
  BufferLayoutData(mBufferWaypoints, mWaypoints.data(), mNumWaypoints);

  ShaderCreateCompute(mShaderComputeShipSteerings, mShaderComputeShipSteeringsSource);
  ShaderCreateCompute(mShaderComputeShipPhysics, mShaderComputeShipPhysicsSource);
  
  ShaderCreateRender(mShaderRenderShips, mShaderRenderShipVertexSource, mShaderRenderShipFragmentSource);
}
void SceneInstancing::OnDisable()
{
  ModelDestroy(mModelShip);

  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  BufferLayoutDestroy(mBufferWaypoints);

  ShaderDestroyCompute(mShaderComputeShipSteerings);
  ShaderDestroyCompute(mShaderComputeShipPhysics);

  ShaderDestroyRender(mShaderRenderShips);
}
void SceneInstancing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);

  ShaderBind(mShaderComputeShipSteerings);
  ShaderUniformR32(mShaderComputeShipSteerings, "uTimeDelta", timeDelta);
  ShaderUniformR32(mShaderComputeShipSteerings, "uSpeed", 10.f);
  ShaderUniformR32(mShaderComputeShipSteerings, "uDecay", 10.f);
  ShaderUniformU32(mShaderComputeShipSteerings, "uMaxWaypoints", mNumWaypoints);

  u32 numThreadsX{ (u32)glm::ceil(mNumShips / 32) };
  ShaderExecuteCompute(mShaderComputeShipSteerings, numThreadsX, 1, 1);
}
void SceneInstancing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);

  ShaderBind(mShaderComputeShipPhysics);

  u32 numThreadsX{ (u32)glm::ceil(mNumShips / 32) };
  ShaderExecuteCompute(mShaderComputeShipPhysics, numThreadsX, 1, 1);
}
void SceneInstancing::OnRender(r32 timeDelta) const
{
  ShaderBind(mShaderRenderShips);
  ShaderUniformR32M4(mShaderRenderShips, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRenderShips, "uView", mCamera.mView);
  ModelRenderInstanced(mModelShip, mNumShips);
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  return;

  for (u32 i{}; i < mNumWaypoints; i++)
  {
    Waypoint const& waypoint{ mWaypoints[i] };

    GizmoLineBatchPushLine(waypoint.mPosition, waypoint.mPositionNext, { 1.f, 1.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeTransforms()
{
  mTransforms.resize(mNumShips);

  for (u32 i{}; i < mNumShips; i++)
  {
    r32v3 position{ glm::ballRand(500.f) };

    mTransforms[i] =
    {
      .mPosition          { position },
      .mRotationLocalRight{ 1.f, 0.f, 0.f },
      .mRotationLocalUp   { 0.f, 1.f, 0.f },
      .mRotationLocalFront{ 0.f, 0.f, 1.f },
    };
  }
}
void SceneInstancing::RandomizeSteerings()
{
  mSteerings.resize(mNumShips);

  for (u32 i{}; i < mNumShips; i++)
  {
    mSteerings[i] =
    {
      .mAcceleration { 0.f, 0.f, 0.f },
      .mVelocity     { 0.f, 0.f, 0.f },
      .mWaypointIndex{ (u32)glm::linearRand(0.f, (r32)mNumWaypoints) },
    };
  }
}
void SceneInstancing::RandomizeWaypoints()
{
  mWaypoints.resize(mNumWaypoints);

  for (u32 i{}; i < mNumWaypoints; i++)
  {
    mWaypoints[i] =
    {
      .mPosition    { glm::ballRand(1500.f) },
      .mPositionNext{ 0.f, 0.f, 0.f },
    };
  }
  for (u32 i{ 1 }; i < mNumWaypoints; i++)
  {
    mWaypoints[i - 1].mPositionNext = { mWaypoints[i % mNumWaypoints].mPosition };
  }

  mWaypoints[mNumWaypoints - 1].mPositionNext = { mWaypoints[0].mPosition };
}