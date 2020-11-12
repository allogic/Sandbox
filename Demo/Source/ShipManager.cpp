#include <ShipManager.h>

ShipManager::ShipManager(
  u32 numShips,
  u32 numOctreeLevels,
  u32 numPaths,
  u32 numPathsSub)
  : mNumShips{ numShips }
  , mNumOctreeLevels{ numOctreeLevels }
  , mNumPaths{ numPaths }
  , mNumPathsSub{ numPathsSub }
{
  ModelCreate(mModelShip, SANDBOX_ROOT_PATH "Model\\Cube.obj");

  ModelLayoutTransform(mModelShip, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });

  UniformLayoutCreate(mUniformSteering);
  UniformLayoutBind(mUniformSteering);
  UniformLayoutMap(mUniformSteering, 1);

  UniformLayoutCreate(mUniformNoise);
  UniformLayoutBind(mUniformNoise);
  UniformLayoutMap(mUniformNoise, 2);

  BufferLayoutCreate(mBufferTransforms, mNumShips);
  BufferLayoutBind(mBufferTransforms);
  BufferLayoutMap(mBufferTransforms, 0);

  BufferLayoutCreate(mBufferSteerings, mNumShips);
  BufferLayoutBind(mBufferSteerings);
  BufferLayoutMap(mBufferSteerings, 1);

  BufferLayoutCreate(mBufferPaths, mNumPaths * mNumPathsSub);
  BufferLayoutBind(mBufferPaths);
  BufferLayoutMap(mBufferPaths, 2);

  BufferLayoutCreate(mBufferOctreeNodes, mNumOctreeLevels);
  BufferLayoutBind(mBufferOctreeNodes);
  BufferLayoutMap(mBufferOctreeNodes, 3);

  InitializeShipTransforms();
  InitializeShipSteerings();
  InitializeShipPaths();
  InitializeOctreeNodes();

  ComputeMaterialCreate(mMaterialComputeShipPhysics, "ShipPhysic");
  ComputeMaterialCreate(mMaterialComputeShipPaths, "ShipPath");
  ComputeMaterialCreate(mMaterialComputeShipOctree, "ShipOctree");

  RenderMaterialCreate(mMaterialLambertInstanced);
}
ShipManager::~ShipManager()
{
  ModelDestroy(mModelShip);

  UniformLayoutDestroy(mUniformSteering);
  UniformLayoutDestroy(mUniformNoise);

  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  BufferLayoutDestroy(mBufferPaths);
  BufferLayoutDestroy(mBufferOctreeNodes);

  ComputeMaterialDestroy(mMaterialComputeShipPhysics);
  ComputeMaterialDestroy(mMaterialComputeShipPaths);
  ComputeMaterialDestroy(mMaterialComputeShipOctree);

  RenderMaterialDestroy(mMaterialLambertInstanced);
}

void ShipManager::OnUpdate(r32 timeDelta)
{
  if (KeyDown(GLFW_KEY_O))
  {
    ComputeMaterialBind(mMaterialComputeShipOctree);
    ComputeMaterialCompute(mMaterialComputeShipOctree, 1, 1, 1);

    BufferLayoutBind(mBufferOctreeNodes);
    BufferLayoutDataGet(mBufferOctreeNodes, mNumOctreeLevels, mOctreeNodes.data());
  }
  if (KeyDown(GLFW_KEY_P))
  {
    ComputeMaterialBind(mMaterialComputeShipPaths);
    ComputeMaterialCompute(mMaterialComputeShipPaths, mNumPaths, 1, 1);

    BufferLayoutBind(mBufferPaths);
    BufferLayoutDataGet(mBufferPaths, mNumPaths * mNumPathsSub, mShipPaths.data());
  }
}
void ShipManager::OnUpdateFixed(r32 timeDelta)
{
  UniformLayoutBind(mUniformSteering);
  Steering steering
  {
    .mTimeDelta        { timeDelta },
    .mAccelerationSpeed{ 2000.f },
    .mVelocityDecay    { 100.f },
    .mMaxPaths         { mNumPaths },
  };
  UniformLayoutDataSet(mUniformSteering, 1, &steering);
  
  UniformLayoutBind(mUniformNoise);
  Noise noise
  {
    .mRandomOffset{ 400.f, 352.f, 784.f },
    .mFrequency   { 0.04f },
  };
  UniformLayoutDataSet(mUniformNoise, 1, &noise);

  ComputeMaterialBind(mMaterialComputeShipPhysics);
  ComputeMaterialCompute(mMaterialComputeShipPhysics, mNumShips / 32, 1, 1);
}
void ShipManager::OnRender(r32 timeDelta)
{
  RenderMaterialBind(mMaterialLambertInstanced);
  ModelRenderInstanced(mModelShip, mNumShips);
}
void ShipManager::OnGizmos(r32 timeDelta)
{
  for (u32 i{}; i < mNumPaths; i++)
  {
    for (u32 j{ 1 }; j < mNumPathsSub; j++)
    {
      ShipWaypoint const& waypointCurr{ mShipPaths[mNumPathsSub * i + j] };
      ShipWaypoint const& waypointPrev{ mShipPaths[mNumPathsSub * i + j - 1] };

      GizmoLineBatchPushLine(waypointCurr.mPosition, waypointPrev.mPosition, { 1.f, 0.f, 0.f, 1.f });
    }
  }
}

void ShipManager::InitializeShipTransforms()
{
  mShipTransforms.resize(mNumShips);

  for (u32 i{}; i < mNumShips; i++)
  {
    r32v3 position{ glm::ballRand(500.f) };

    mShipTransforms[i] =
    {
      .mPosition          { position },
      .mRotationLocalRight{ 1.f, 0.f, 0.f },
      .mRotationLocalUp   { 0.f, 1.f, 0.f },
      .mRotationLocalFront{ 0.f, 0.f, 1.f },
    };
  }

  BufferLayoutBind(mBufferTransforms);
  BufferLayoutDataSet(mBufferTransforms, mNumShips, mShipTransforms.data());
}
void ShipManager::InitializeShipSteerings()
{
  mShipSteerings.resize(mNumShips);

  for (u32 i{}; i < mNumShips; i++)
  {
    mShipSteerings[i] =
    {
      .mAcceleration{ 0.f, 0.f, 0.f },
      .mVelocity    { 0.f, 0.f, 0.f },
      .mPathIndex   { (u32)glm::floor(glm::linearRand(0.f, (r32)mNumPaths)) },
      .mPathIndexSub{ 0 },
    };
  }

  BufferLayoutBind(mBufferSteerings);
  BufferLayoutDataSet(mBufferSteerings, mNumShips, mShipSteerings.data());
}
void ShipManager::InitializeShipPaths()
{
  mShipPaths.resize(mNumPaths * mNumPathsSub);

  for (u32 i{}; i < mNumPaths; i++)
  {
    r32v2 positionRandom{ glm::circularRand(500.f) };

    mShipPaths[mNumPathsSub * i].mPosition = { positionRandom.x, 0, positionRandom.y };

    for (u32 j{ 1 }; j < mNumPathsSub; j++)
    {
      r32v3 positionWaypointPrev{ mShipPaths[mNumPathsSub * i + j - 1].mPosition };

      mShipPaths[mNumPathsSub * i + j].mPosition = positionWaypointPrev - glm::normalize(positionWaypointPrev);
    }
  }

  BufferLayoutBind(mBufferPaths);
  BufferLayoutDataSet(mBufferPaths, mNumPaths * mNumPathsSub, mShipPaths.data());
}
void ShipManager::InitializeOctreeNodes()
{
  mOctreeNodes.resize(mNumOctreeLevels);

  BufferLayoutBind(mBufferOctreeNodes);
  BufferLayoutDataSet(mBufferOctreeNodes, mNumOctreeLevels, mOctreeNodes.data());
}