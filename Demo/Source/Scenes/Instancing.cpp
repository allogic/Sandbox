#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 500.f }, 45.f, 0.001f, 10000.f);
  ModelCreate(mModelSpaceShip, "C:\\Users\\Burmi\\Downloads\\Sandbox\\Models\\cube.fbx");
  RandomizeTransforms();
  RandomizeSteerings();
  RandomizeWaypoints();
  BufferLayoutCreate(mBufferTransforms, mNumSpaceShips, 0);
  BufferLayoutCreate(mBufferSteerings, mNumSpaceShips, 1);
  BufferLayoutCreate(mBufferWaypoints, mNumWaypoints, 2);
  BufferLayoutData(mBufferTransforms, mTransforms.data(), mNumSpaceShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), mNumSpaceShips);
  BufferLayoutData(mBufferWaypoints, mWaypoints.data(), mNumWaypoints);
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

  ShaderBind(mShaderCompute);
  ShaderUniformR32(mShaderCompute, "uTimeDelta", timeDelta);
  ShaderUniformR32(mShaderCompute, "uAccelerationSpeed", 0.02f);
  ShaderUniformR32(mShaderCompute, "uVelocityDecay", 1.f);
  ShaderUniformU32(mShaderCompute, "uMaxWaypoints", mNumWaypoints);
  ShaderExecuteCompute(mShaderCompute, (u32)glm::ceil(mNumSpaceShips / 32), 1, 1);
}
void SceneInstancing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);
}
void SceneInstancing::OnRender(r32 timeDelta) const
{
  ShaderBind(mShaderRender);
  ShaderUniformR32M4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRender, "uView", mCamera.mView);
  ModelRenderInstanced(mModelSpaceShip, mNumSpaceShips);
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  for (u32 i{}; i < mNumWaypoints; i++)
  {
    Waypoint const& waypoint{ mWaypoints[i] };

    GizmoLineBatchPushLine(waypoint.mPosition, waypoint.mPositionNext, { 1.f, 1.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeTransforms()
{
  mTransforms.resize(mNumSpaceShips);

  for (u32 i{}; i < mNumSpaceShips; i++)
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
  mSteerings.resize(mNumSpaceShips);

  for (u32 i{}; i < mNumSpaceShips; i++)
  {
    mSteerings[i] =
    {
      .mAcceleration { 0.f, 0.f, 0.f },
      .mVelocity     { 0.f, 0.f, 0.f },
      .mWaypointIndex{ 1 },
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
      .mPosition    { glm::ballRand(500.f) },
      .mPositionNext{ 0.f, 0.f, 0.f },
    };
  }
  for (u32 i{ 1 }; i < mNumWaypoints; i++)
  {
    mWaypoints[i - 1].mPositionNext = { mWaypoints[i % mNumWaypoints].mPosition };
  }

  mWaypoints[mNumWaypoints - 1].mPositionNext = { mWaypoints[0].mPosition };
}