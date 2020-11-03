#include <Scenes/Game.h>

void SceneGame::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 10000.f);

  ModelCreate(mModelShip, SANDBOX_ROOT_PATH "Model\\Cube.obj");
  ModelCreate(mModelSky, SANDBOX_ROOT_PATH "Model\\Sphere.obj");
  ModelCreate(mModelMap, SANDBOX_ROOT_PATH "Model\\Plane.obj");

  ModelLayoutTransform(mModelShip, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });
  ModelLayoutTransform(mModelSky, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1000.f, 1000.f, 1000.f });
  ModelLayoutTransform(mModelMap, { 0.f, -10.f, 0.f }, { 0.f, 0.f, 0.f }, { 1000.f, 1.f, 1000.f });

  TextureLayoutCreate(mTextureSky, 4096, 4096);
  TextureLayoutCreate(mTextureMap, 4096, 4096);

  InitializeTransforms();
  InitializeSteerings();
  InitializePaths();

  BufferLayoutCreate(mBufferTransforms, mNumShips, 0);
  BufferLayoutCreate(mBufferSteerings, mNumShips, 1);
  BufferLayoutCreate(mBufferPaths, mNumPaths * mNumPathSub, 2);

  BufferLayoutData(mBufferTransforms, mTransforms.data(), mNumShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), mNumShips);
  BufferLayoutData(mBufferPaths, mPaths.data(), mNumPaths * mNumPathSub);

  ShaderLayoutCreate(mShaderComputeShipSteerings, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipSteering.comp" } });
  ShaderLayoutCreate(mShaderComputeShipPhysics, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipPhysic.comp" } });
  ShaderLayoutCreate(mShaderComputeShipPaths, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipPath.comp" } });
  ShaderLayoutCreate(mShaderComputeMapNoise, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\MapNoise.comp" } });

  ShaderLayoutCreate(mShaderRenderShips, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Ship\\Ship.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Ship\\Ship.frag" } });
  ShaderLayoutCreate(mShaderRenderSky, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" } });
  ShaderLayoutCreate(mShaderRenderMap, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" } });
}
void SceneGame::OnDisable()
{
  ModelDestroy(mModelShip);
  ModelDestroy(mModelSky);
  ModelDestroy(mModelMap);

  TextureLayoutDestroy(mTextureSky);
  TextureLayoutDestroy(mTextureMap);

  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  BufferLayoutDestroy(mBufferPaths);

  ShaderLayoutDestroy(mShaderComputeShipSteerings);
  ShaderLayoutDestroy(mShaderComputeShipPhysics);
  ShaderLayoutDestroy(mShaderComputeShipPaths);
  ShaderLayoutDestroy(mShaderComputeMapNoise);

  ShaderLayoutDestroy(mShaderRenderShips);
  ShaderLayoutDestroy(mShaderRenderSky);
  ShaderLayoutDestroy(mShaderRenderMap);
}
void SceneGame::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);

  if (KeyDown(GLFW_KEY_P))
  {
    ShaderLayoutBind(mShaderComputeShipPaths);
    ShaderLayoutUniformR32V3(mShaderComputeShipPaths, "uOffsetRandom", glm::ballRand(10000.f));
    ShaderLayoutCompute(mShaderComputeShipPaths, mNumPathSub, 1, 1);
  }
  if (KeyDown(GLFW_KEY_M))
  {
    TextureLayoutBind(mTextureMap);
    TextureLayoutBindImage(mTextureMap, 1);
    ShaderLayoutBind(mShaderComputeMapNoise);
    ShaderLayoutUniformR32V3(mShaderComputeMapNoise, "uOffsetRandom", glm::ballRand(10000.f));
    ShaderLayoutCompute(mShaderComputeMapNoise, 4096 / 32, 4096 / 32, 1);
  }

  ShaderLayoutBind(mShaderComputeShipSteerings);
  ShaderLayoutUniformR32(mShaderComputeShipSteerings, "uTimeDelta", timeDelta);
  ShaderLayoutUniformR32(mShaderComputeShipSteerings, "uAccelerationSpeed", 2000.f);
  ShaderLayoutUniformR32(mShaderComputeShipSteerings, "uVelocityDecay", 100.f);
  ShaderLayoutUniformU32(mShaderComputeShipSteerings, "uMaxPaths", mNumPaths);

  u32 numThreadsX{ (u32)glm::ceil(mNumShips / 32) };
  ShaderLayoutCompute(mShaderComputeShipSteerings, numThreadsX, 1, 1);
}
void SceneGame::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);

  ShaderLayoutBind(mShaderComputeShipPhysics);

  u32 numThreadsX{ (u32)glm::ceil(mNumShips / 32) };
  ShaderLayoutCompute(mShaderComputeShipPhysics, numThreadsX, 1, 1);
}
void SceneGame::OnRender(r32 timeDelta) const
{
  ShaderLayoutBind(mShaderRenderShips);
  ShaderLayoutUniformR32M4(mShaderRenderShips, "uProjection", mCamera.mProjection);
  ShaderLayoutUniformR32M4(mShaderRenderShips, "uView", mCamera.mView);
  //ModelRenderInstanced(mModelShip, mNumShips);

  //TextureLayoutBind(mTextureSky);
  //TextureLayoutBindSampler(mTextureSky, 1);
  ShaderLayoutBind(mShaderRenderSky);
  ShaderLayoutUniformR32M4(mShaderRenderSky, "uProjection", mCamera.mProjection);
  ShaderLayoutUniformR32M4(mShaderRenderSky, "uView", mCamera.mView);
  ShaderLayoutUniformR32M4(mShaderRenderSky, "uTransform", mModelSky.mTransform);
  //ModelRender(mModelSky);

  //TextureLayoutBind(mTextureMap);
  //TextureLayoutBindSampler(mTextureMap, 1);
  //ShaderBind(mShaderRenderMap);
  //ShaderUniformR32M4(mShaderRenderMap, "uProjection", mCamera.mProjection);
  //ShaderUniformR32M4(mShaderRenderMap, "uView", mCamera.mView);
  //ShaderUniformR32M4(mShaderRenderMap, "uTransform", mModelMap.mTransform);
  //ModelRender(mModelMap);
}
void SceneGame::OnGizmos(r32 timeDelta)
{
  return;

  BufferLayoutDataSubGet(mBufferPaths, 0, mNumPaths * mNumPathSub, mPaths.data());

  for (u32 i{}; i < mNumPaths; i++)
  {
    for (u32 j{ 1 }; j < 100; j++)
    {
      Waypoint const& waypointCurr{ mPaths[mNumPathSub * i + j] };
      Waypoint const& waypointPrev{ mPaths[mNumPathSub * i + j - 1] };

      GizmoLineBatchPushLine(waypointCurr.mPosition, waypointPrev.mPosition, { 1.f, 0.f, 0.f, 1.f });
      GizmoLineBatchPushLine(waypointCurr.mPosition, waypointCurr.mPosition + waypointCurr.mDirection, { 1.f, 1.f, 0.f, 1.f });
    }
  }

  u32 size{ 32 };

  for (u32 i{}; i <= size; i++)
    for (u32 j{}; j <= size; j++)
    {
      r32v3 spacing{ 100.f, 100.f, 100.f };
      r32v3 half{ size / 2, 0.f, size / 2 };
      r32v3 startP0{ i, 0, 0 };
      r32v3 startP1{ 0, 0, j };
      r32v3 endP0{ i, 0, size };
      r32v3 endP1{ size, 0, j };
      r32v4 color{ (i / (r32)size), (j / (r32)size), 0.f, 1.f };

      startP0 *= spacing;
      startP1 *= spacing;
      startP0 += -half * spacing;
      startP1 += -half * spacing;

      endP0 *= spacing;
      endP1 *= spacing;
      endP0 += -half * spacing;
      endP1 += -half * spacing;

      GizmoLineBatchPushLine(startP0, endP0, color);
      GizmoLineBatchPushLine(startP1, endP1, color);
    }
}

void SceneGame::InitializeTransforms()
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
void SceneGame::InitializeSteerings()
{
  mSteerings.resize(mNumShips);

  for (u32 i{}; i < mNumShips; i++)
  {
    mSteerings[i] =
    {
      .mAcceleration{ 0.f, 0.f, 0.f },
      .mVelocity    { 0.f, 0.f, 0.f },
      .mPathIndex   { (u32)glm::floor(glm::linearRand(0.f, (r32)mNumPaths)) },
      .mPathIndexSub{ 0 },
    };
  }
}
void SceneGame::InitializePaths()
{
  mPaths.resize(mNumPaths * mNumPathSub);

  for (u32 i{}; i < mNumPaths; i++)
  {
    r32v3 positionWaypoint{ i * 10.f, 0.f, 0.f };

    mPaths[mNumPathSub * i].mPosition = positionWaypoint;
    mPaths[mNumPathSub * i].mDirection = { 0.f, 0.f, 1.f };
    mPaths[mNumPathSub * i].mRotationLocalFront = { 0.f, 0.f, 1.f };

    for (u32 j{ 1 }; j < mNumPathSub; j++)
    {
      r32v3 positionWaypointPrev{ mPaths[mNumPathSub * i + j - 1].mPosition };
      r32v3 directionWaypointPrev{ mPaths[mNumPathSub * i + j - 1].mDirection };

      mPaths[mNumPathSub * i + j].mPosition = positionWaypointPrev + directionWaypointPrev;
      mPaths[mNumPathSub * i + j].mDirection = directionWaypointPrev;
      mPaths[mNumPathSub * i + j].mRotationLocalFront = directionWaypointPrev;
    }
  }
}