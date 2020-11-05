#include <Scenes/Game.h>

void SceneGame::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, WindowAspect(), 45.f, 0.001f, 10000.f);

  ModelCreate(mModelShip, SANDBOX_ROOT_PATH "Model\\Cube.obj");
  ModelCreate(mModelMap, SANDBOX_ROOT_PATH "Model\\Plane.obj");

  ModelLayoutTransform(mModelShip, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });
  ModelLayoutTransform(mModelMap, { 0.f, -10.f, 0.f }, { 0.f, 0.f, 0.f }, { 1000.f, 1.f, 1000.f });

  TextureLayoutCreate(mTextureMap, mNumMapDimension, mNumMapDimension);

  InitializeTransforms();
  InitializeSteerings();
  InitializePaths();

  BufferLayoutCreate(mBufferTransforms, mNumShips, 0);
  BufferLayoutCreate(mBufferSteerings, mNumShips, 1);
  BufferLayoutCreate(mBufferPaths, mNumPathsTotal, 2);

  BufferLayoutBind(mBufferTransforms);
  BufferLayoutDataSet(mBufferTransforms, mNumShips, mTransforms.data());

  BufferLayoutBind(mBufferSteerings);
  BufferLayoutDataSet(mBufferSteerings, mNumShips, mSteerings.data());

  BufferLayoutBind(mBufferPaths);
  BufferLayoutDataSet(mBufferPaths, mNumPathsTotal, mPaths.data());

  UniformLayoutCreate(mUniformConfigSteering, 0);
  UniformLayoutCreate(mUniformConfigNoise, 1);
  UniformLayoutCreate(mUniformConfigProjection, 2);

  UniformLayoutBind(mUniformConfigSteering);
  UniformLayoutDataSet(mUniformConfigSteering, 1, &mConfigSteering);

  UniformLayoutBind(mUniformConfigNoise);
  UniformLayoutDataSet(mUniformConfigNoise, 1, &mConfigNoise);

  UniformLayoutBind(mUniformConfigProjection);
  UniformLayoutDataSet(mUniformConfigProjection, 1, &mConfigProjection);

  ShaderLayoutCreate(mShaderComputeShipSteerings, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipSteering.comp" } });
  ShaderLayoutCreate(mShaderComputeShipPhysics, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipPhysic.comp" } });
  ShaderLayoutCreate(mShaderComputeShipPaths, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\ShipPath.comp" } });
  ShaderLayoutCreate(mShaderComputeMapNoise, ShaderPaths{ .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\MapNoise.comp" } });

  ShaderLayoutCreate(mShaderRenderShips, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\LambertInstanced\\LambertInstanced.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\LambertInstanced\\LambertInstanced.frag" } });
  ShaderLayoutCreate(mShaderRenderMap, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" } });
}
void SceneGame::OnDisable()
{
  ModelDestroy(mModelShip);
  ModelDestroy(mModelMap);

  TextureLayoutDestroy(mTextureMap);

  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  BufferLayoutDestroy(mBufferPaths);

  UniformLayoutDestroy(mUniformConfigSteering);
  UniformLayoutDestroy(mUniformConfigNoise);
  UniformLayoutDestroy(mUniformConfigProjection);

  ShaderLayoutDestroy(mShaderComputeShipSteerings);
  ShaderLayoutDestroy(mShaderComputeShipPhysics);
  ShaderLayoutDestroy(mShaderComputeShipPaths);
  ShaderLayoutDestroy(mShaderComputeMapNoise);

  ShaderLayoutDestroy(mShaderRenderShips);
  ShaderLayoutDestroy(mShaderRenderMap);
}
void SceneGame::OnUpdate(r32 timeDelta)
{
  //CameraControllerUpdateInputSpace(timeDelta);
  CameraControllerUpdateInputOrbit(timeDelta);

  mConfigSteering.mTimeDelta = timeDelta;
  mConfigSteering.mAccelerationSpeed = 2000.f;
  mConfigSteering.mVelocityDecay = 100.f;
  mConfigSteering.mMaxPaths = mNumPaths;

  UniformLayoutBind(mUniformConfigSteering);
  UniformLayoutDataSet(mUniformConfigSteering, 1, &mConfigSteering);

  mConfigNoise.mRandomOffset = glm::ballRand(100.f);

  UniformLayoutBind(mUniformConfigNoise);
  UniformLayoutDataSet(mUniformConfigNoise, 1, &mConfigNoise);

  if (KeyDown(GLFW_KEY_P))
  {
    ShaderLayoutBind(mShaderComputeShipPaths);
    ShaderLayoutCompute(mShaderComputeShipPaths, mNumPathsSub, 1, 1);
  }
  if (KeyDown(GLFW_KEY_M))
  {
    TextureLayoutBind(mTextureMap);
    TextureLayoutBindImage(mTextureMap, 0);
    ShaderLayoutBind(mShaderComputeMapNoise);
    ShaderLayoutCompute(mShaderComputeMapNoise, mNumMapDimension / 32, mNumMapDimension / 32, 1);
  }
}
void SceneGame::OnUpdateFixed(r32 timeDelta)
{
  //CameraControllerUpdatePhysicsSpace(mCamera, mCameraControllerSpace);
  CameraControllerUpdatePhysicsOrbit(mCamera, mCameraControllerOrbit);

  ShaderLayoutBind(mShaderComputeShipSteerings);
  ShaderLayoutCompute(mShaderComputeShipSteerings, mNumShips / 32, 1, 1);

  ShaderLayoutBind(mShaderComputeShipPhysics);
  ShaderLayoutCompute(mShaderComputeShipPhysics, mNumShips / 32, 1, 1);
}
void SceneGame::OnRender(r32 timeDelta)
{
  mConfigProjection.mProjection = mCamera.mProjection;
  mConfigProjection.mView = mCamera.mView;
  mConfigProjection.mTransform = glm::identity<r32m4>();

  UniformLayoutBind(mUniformConfigProjection);
  UniformLayoutDataSet(mUniformConfigProjection, 1, &mConfigProjection);

  ShaderLayoutBind(mShaderRenderShips);
  ModelRenderInstanced(mModelShip, mNumShips);

  mConfigProjection.mTransform = mModelMap.mTransform;

  UniformLayoutBind(mUniformConfigProjection);
  UniformLayoutDataSet(mUniformConfigProjection, 1, &mConfigProjection);

  TextureLayoutBind(mTextureMap);
  TextureLayoutBindSampler(mTextureMap, 0);
  ShaderLayoutBind(mShaderRenderMap);
  ModelRender(mModelMap);
}
void SceneGame::OnGizmos(r32 timeDelta)
{
  BufferLayoutBind(mBufferPaths);
  BufferLayoutDataGet(mBufferPaths, mNumPathsTotal, mPaths.data());

  for (u32 i{}; i < mNumPaths; i++)
  {
    for (u32 j{ 1 }; j < 100; j++)
    {
      Waypoint const& waypointCurr{ mPaths[mNumPathsSub * i + j] };
      Waypoint const& waypointPrev{ mPaths[mNumPathsSub * i + j - 1] };

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
      r32v4 color{ 1.f, 1.f, 0.f, 1.f };

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

void SceneGame::CameraControllerUpdateInputSpace(r32 timeDelta)
{
  if (KeyHeld(GLFW_KEY_A)) mCameraControllerSpace.mPositionAccel += -glm::normalize(mCamera.mLocalRight) * mCameraControllerSpace.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) mCameraControllerSpace.mPositionAccel += glm::normalize(mCamera.mLocalRight) * mCameraControllerSpace.mPositionAmount * timeDelta;

  if (KeyHeld(GLFW_KEY_S)) mCameraControllerSpace.mPositionAccel += -glm::normalize(mCamera.mLocalFront) * mCameraControllerSpace.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) mCameraControllerSpace.mPositionAccel += glm::normalize(mCamera.mLocalFront) * mCameraControllerSpace.mPositionAmount * timeDelta;

  if (KeyHeld(GLFW_KEY_Q)) mCameraControllerSpace.mRollAccel += -mCameraControllerSpace.mRollAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_E)) mCameraControllerSpace.mRollAccel += mCameraControllerSpace.mRollAmount * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      mCameraControllerSpace.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mCameraControllerSpace.mRotationAmount * timeDelta;
  }

  mCameraControllerSpace.mPositionVelocity += -mCameraControllerSpace.mPositionVelocity * mCameraControllerSpace.mPositionVelocityDecay * timeDelta;
  mCameraControllerSpace.mRotationVelocityAngular += -mCameraControllerSpace.mRotationVelocityAngular * mCameraControllerSpace.mRotationVelocityDecay * timeDelta;
  mCameraControllerSpace.mRollVelocityAngular += -mCameraControllerSpace.mRollVelocityAngular * mCameraControllerSpace.mRollVelocityDecay * timeDelta;
}
void SceneGame::CameraControllerUpdateInputOrbit(r32 timeDelta)
{
  r32 distanceScaleLog{ glm::log(mCameraControllerOrbit.mScrollDistance) };

  if (KeyHeld(GLFW_KEY_A)) mCameraControllerOrbit.mPositionAccel += -glm::normalize(mCamera.mLocalRight) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) mCameraControllerOrbit.mPositionAccel += glm::normalize(mCamera.mLocalRight) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;

  r32v3 mLocalFrontNoXRot{ glm::cross(mCamera.mUp, mCamera.mLocalRight) };

  if (KeyHeld(GLFW_KEY_S)) mCameraControllerOrbit.mPositionAccel += -glm::normalize(mLocalFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) mCameraControllerOrbit.mPositionAccel += glm::normalize(mLocalFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      mCameraControllerOrbit.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mCameraControllerOrbit.mRotationAmount * distanceScaleLog * timeDelta;
  }

  mCameraControllerOrbit.mScrollAccel += -MouseScrollY() * mCameraControllerOrbit.mScrollAmount * timeDelta;

  mCameraControllerOrbit.mPositionVelocity += -mCameraControllerOrbit.mPositionVelocity * mCameraControllerOrbit.mPositionVelocityDecay * timeDelta;
  mCameraControllerOrbit.mRotationVelocityAngular += -mCameraControllerOrbit.mRotationVelocityAngular * mCameraControllerOrbit.mRotationVelocityDecay * timeDelta;
  mCameraControllerOrbit.mScrollVelocity += -mCameraControllerOrbit.mScrollVelocity * mCameraControllerOrbit.mScrollVelocityDecay * timeDelta;

  mCameraControllerOrbit.mScrollVelocity = glm::clamp(mCameraControllerOrbit.mScrollVelocity, -2.f, 2.f);
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
  mPaths.resize(mNumPathsTotal);

  for (u32 i{}; i < mNumPaths; i++)
  {
    r32v3 positionWaypoint{ i * 10.f, 0.f, 0.f };

    mPaths[mNumPathsSub * i].mPosition = positionWaypoint;
    mPaths[mNumPathsSub * i].mDirection = { 0.f, 0.f, 1.f };
    mPaths[mNumPathsSub * i].mRotationLocalFront = { 0.f, 0.f, 1.f };

    for (u32 j{ 1 }; j < mNumPathsSub; j++)
    {
      r32v3 positionWaypointPrev{ mPaths[mNumPathsSub * i + j - 1].mPosition };
      r32v3 directionWaypointPrev{ mPaths[mNumPathsSub * i + j - 1].mDirection };

      mPaths[mNumPathsSub * i + j].mPosition = positionWaypointPrev + directionWaypointPrev;
      mPaths[mNumPathsSub * i + j].mDirection = directionWaypointPrev;
      mPaths[mNumPathsSub * i + j].mRotationLocalFront = directionWaypointPrev;
    }
  }
}