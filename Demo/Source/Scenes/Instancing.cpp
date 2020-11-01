#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 500.f }, 45.f, 0.001f, 10000.f);
  ModelCreate(mModelShip, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\cube.fbx");

  InitializeTransforms();
  InitializeSteerings();
  InitializePaths();

  BufferLayoutCreate(mBufferTransforms, mNumShips, 0);
  BufferLayoutCreate(mBufferSteerings, mNumShips, 1);
  BufferLayoutCreate(mBufferPaths, mNumPaths, 2);

  BufferLayoutData(mBufferTransforms, mTransforms.data(), mNumShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), mNumShips);
  BufferLayoutData(mBufferPaths, mPaths.data(), mNumPaths);

  ShaderCreateCompute(mShaderComputeShipSteerings, mShaderComputeShipSteeringsSource);
  ShaderCreateCompute(mShaderComputeShipPhysics, mShaderComputeShipPhysicsSource);
  ShaderCreateCompute(mShaderComputeMap, mShaderComputeMapSource);

  ShaderCreateRender(mShaderRenderShips, mShaderRenderShipVertexSource, mShaderRenderShipFragmentSource);
}
void SceneInstancing::OnDisable()
{
  ModelDestroy(mModelShip);

  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  BufferLayoutDestroy(mBufferPaths);

  ShaderDestroyCompute(mShaderComputeShipSteerings);
  ShaderDestroyCompute(mShaderComputeShipPhysics);

  ShaderDestroyRender(mShaderRenderShips);
}
void SceneInstancing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);

  if (KeyDown(GLFW_KEY_G))
  {
    ShaderBind(mShaderComputeMap);
    ShaderUniformR32(mShaderComputeMap, "uWindowSizeX", WindowSizeX());
    ShaderUniformR32V3(mShaderComputeMap, "uOffsetRandom", glm::ballRand(10000.f));

    u32 numThreadsX{ (u32)glm::ceil(mNumPaths / 32) };
    ShaderExecuteCompute(mShaderComputeMap, numThreadsX, 1, 1);
  }

  ShaderBind(mShaderComputeShipSteerings);
  ShaderUniformR32(mShaderComputeShipSteerings, "uTimeDelta", timeDelta);
  ShaderUniformR32(mShaderComputeShipSteerings, "uAccelerationSpeed", 2000.f);
  ShaderUniformR32(mShaderComputeShipSteerings, "uVelocityDecay", 100.f);
  ShaderUniformU32(mShaderComputeShipSteerings, "uMaxPaths", mNumPaths);

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
  static std::vector<Path> path{};

  path.resize(1024 * 32);
  BufferLayoutDataSubGet(mBufferPaths, 0, 1024 * 32, path.data());

  for (u32 i{}; i < 32; i++)
  {
    for (u32 j{ 1 }; j < 1024; j++)
    {
      r32v3 positionPrev{ path[i + j * 32].mPosition };
      r32v3 positionCurr{ path[i + j * 32].mPosition };

      GizmoLineBatchPushLine(positionPrev, positionCurr, { 1.f, 0.f, 0.f, 1.f });
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

void SceneInstancing::InitializeTransforms()
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
void SceneInstancing::InitializeSteerings()
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
void SceneInstancing::InitializePaths()
{
  mPaths.resize(mNumPaths);

  for (u32 i{}; i < mNumPaths; i++)
  {
    r32v2 circularPosition{ glm::circularRand(500.f) };

    mPaths[i] =
    {
      .mPosition{ circularPosition.x, 0.f, circularPosition.y },
    };
  }
}