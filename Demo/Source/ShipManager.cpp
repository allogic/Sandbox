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

  //UniformLayoutCreate(mUniformSteering);
  //UniformLayoutBind(mUniformSteering);
  //UniformLayoutMap(mUniformSteering, 1);
  //
  //UniformLayoutCreate(mUniformNoise);
  //UniformLayoutBind(mUniformNoise);
  //UniformLayoutMap(mUniformNoise, 2);

  BufferLayoutCreate(mBufferSteering, mNumShips);
  BufferLayoutBind(mBufferSteering);
  BufferLayoutMap(mBufferSteering, 1);
  
  BufferLayoutCreate(mBufferPath, mNumPaths * mNumPathsSub);
  BufferLayoutBind(mBufferPath);
  BufferLayoutMap(mBufferPath, 2);
  
  BufferLayoutCreate(mBufferOctreeNode, mNumOctreeLevels);
  BufferLayoutBind(mBufferOctreeNode);
  BufferLayoutMap(mBufferOctreeNode, 3);

  ComputeMaterialCreate(mMaterialComputeShipPhysics, "ShipPhysic");
  ComputeMaterialCreate(mMaterialComputeShipPaths, "ShipPath");
  ComputeMaterialCreate(mMaterialComputeShipOctree, "ShipOctree");

  RenderMaterialCreate(mMaterialLambertInstanced, mNumShips);

  InitializeShipTransforms();
  InitializeShipSteerings();
  InitializeShipPaths();
  InitializeOctreeNodes();
}
ShipManager::~ShipManager()
{
  ModelDestroy(mModelShip);

  UniformLayoutDestroy(mUniformSteering);
  UniformLayoutDestroy(mUniformNoise);

  BufferLayoutDestroy(mBufferTransform);
  BufferLayoutDestroy(mBufferSteering);
  BufferLayoutDestroy(mBufferPath);
  BufferLayoutDestroy(mBufferOctreeNode);

  ComputeMaterialDestroy(mMaterialComputeShipPhysics);
  ComputeMaterialDestroy(mMaterialComputeShipPaths);
  ComputeMaterialDestroy(mMaterialComputeShipOctree);

  RenderMaterialDestroy(mMaterialLambertInstanced);
}

void ShipManager::Update(r32 timeDelta)
{
  if (KeyDown(GLFW_KEY_O))
  {
    ComputeMaterialBind(mMaterialComputeShipOctree);
    ComputeMaterialCompute(mMaterialComputeShipOctree);

    BufferLayoutBind(mBufferOctreeNode);
    BufferLayoutDataGet(mBufferOctreeNode, mNumOctreeLevels, mOctreeNodes.data());
  }
  if (KeyDown(GLFW_KEY_P))
  {
    ComputeMaterialBind(mMaterialComputeShipPaths);
    ComputeMaterialCompute(mMaterialComputeShipPaths);

    BufferLayoutBind(mBufferPath);
    BufferLayoutDataGet(mBufferPath, mNumPaths * mNumPathsSub, mShipPaths.data());
  }
}
void ShipManager::UpdatePhysics(r32 timeDelta)
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
  ComputeMaterialCompute(mMaterialComputeShipPhysics);
}
void ShipManager::Render()
{
  RenderMaterialBind(mMaterialLambertInstanced);
  ModelRenderInstanced(mModelShip, mNumShips);
}
void ShipManager::Debug()
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

  BufferLayoutBind(mBufferTransform);
  BufferLayoutDataSet(mBufferTransform, mNumShips, mShipTransforms.data());
  BufferLayoutUnbind(mBufferTransform);
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

  BufferLayoutBind(mBufferSteering);
  BufferLayoutDataSet(mBufferSteering, mNumShips, mShipSteerings.data());
  BufferLayoutUnbind(mBufferSteering);
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

  BufferLayoutBind(mBufferPath);
  BufferLayoutDataSet(mBufferPath, mNumPaths * mNumPathsSub, mShipPaths.data());
  BufferLayoutUnbind(mBufferPath);
}
void ShipManager::InitializeOctreeNodes()
{
  mOctreeNodes.resize(mNumOctreeLevels);

  BufferLayoutBind(mBufferOctreeNode);
  BufferLayoutDataSet(mBufferOctreeNode, mNumOctreeLevels, mOctreeNodes.data());
  BufferLayoutUnbind(mBufferOctreeNode);
}