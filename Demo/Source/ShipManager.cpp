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
  ModelCreate(mModelShip, SANDBOX_ENGINE_ROOT_PATH "Model\\Cube.obj");
  ModelLayoutTransform(mModelShip, { 0.f, 20.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });

  TextureLayoutCreate(mTextureShip, 512, 512);
  TextureLayoutBind(mTextureShip);
  TextureLayoutDataSetFrom(mTextureShip, SANDBOX_ENGINE_ROOT_PATH "Texture\\UV.png");

  BufferLayoutCreate(mBufferShipTransform, mNumShips);
  BufferLayoutCreate(mBufferShipSteering, mNumShips);
  BufferLayoutCreate(mBufferShipPath, mNumPaths * mNumPathsSub);
  BufferLayoutCreate(mBufferShipOctree, mNumOctreeLevels);

  UniformLayoutCreate(mUniformSteering);
  UniformLayoutCreate(mUniformNoise);

  ComputeMaterialCreate(mMaterialComputeShipPhysics, "ShipPhysic");
  ComputeMaterialCreate(mMaterialComputeShipPaths, "ShipPath");
  ComputeMaterialCreate(mMaterialComputeShipOctree, "ShipOctree");

  InitializeShipTransforms();
  InitializeShipSteerings();
  InitializeShipPaths();
  InitializeOctreeNodes();
}
ShipManager::~ShipManager()
{
  ModelDestroy(mModelShip);
  TextureLayoutDestroy(mTextureShip);

  UniformLayoutDestroy(mUniformSteering);
  UniformLayoutDestroy(mUniformNoise);

  BufferLayoutDestroy(mBufferShipTransform);
  BufferLayoutDestroy(mBufferShipSteering);
  BufferLayoutDestroy(mBufferShipPath);
  BufferLayoutDestroy(mBufferShipOctree);

  ComputeMaterialDestroy(mMaterialComputeShipPhysics);
  ComputeMaterialDestroy(mMaterialComputeShipPaths);
  ComputeMaterialDestroy(mMaterialComputeShipOctree);
}

void ShipManager::Update(r32 timeDelta)
{
  if (KeyDown(GLFW_KEY_O))
  {
    ComputeMaterialBind(mMaterialComputeShipOctree);
    ComputeMaterialCompute(mMaterialComputeShipOctree);

    BufferLayoutBind(mBufferShipOctree);
    BufferLayoutDataGet(mBufferShipOctree, mNumOctreeLevels, mOctreeNodes.data());
  }
  if (KeyDown(GLFW_KEY_P))
  {
    ComputeMaterialBind(mMaterialComputeShipPaths);
    ComputeMaterialCompute(mMaterialComputeShipPaths);

    BufferLayoutBind(mBufferShipPath);
    BufferLayoutDataGet(mBufferShipPath, mNumPaths * mNumPathsSub, mShipPaths.data());
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

  BufferLayoutBind(mBufferShipTransform);
  BufferLayoutMap(mBufferShipTransform, 0);

  BufferLayoutBind(mBufferShipSteering);
  BufferLayoutMap(mBufferShipSteering, 1);

  BufferLayoutBind(mBufferShipPath);
  BufferLayoutMap(mBufferShipPath, 2);

  BufferLayoutBind(mBufferShipOctree);
  BufferLayoutMap(mBufferShipOctree, 3);

  UniformLayoutBind(mUniformSteering);
  UniformLayoutMap(mUniformSteering, 1);

  ComputeMaterialBind(mMaterialComputeShipPhysics);
  ComputeMaterialCompute(mMaterialComputeShipPhysics);
}
void ShipManager::Render()
{
  RendererSubmitLambertInstanced(mRenderer, TaskLambertInstanced
  {
    &mModelShip,
    &mBufferShipTransform,
    &mTextureShip,
    mNumShips
  });
}
void ShipManager::Debug()
{
  for (u32 i{}; i < mNumPaths; i++)
  {
    for (u32 j{ 1 }; j < mNumPathsSub; j++)
    {
      ShipWaypoint const& waypointCurr{ mShipPaths[mNumPathsSub * i + j] };
      ShipWaypoint const& waypointPrev{ mShipPaths[mNumPathsSub * i + j - 1] };
  
      RendererLineBatchPushLine(mRenderer, waypointCurr.mPosition, waypointPrev.mPosition, { 1.f, 0.f, 0.f, 1.f });
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
      .mRotation          { 45.f, 45.f, 45.f },
      .mRotationLocalRight{ 1.f, 0.f, 0.f },
      .mRotationLocalUp   { 0.f, 1.f, 0.f },
      .mRotationLocalFront{ 0.f, 0.f, 1.f },
    };
  }

  BufferLayoutBind(mBufferShipTransform);
  BufferLayoutDataSet(mBufferShipTransform, mNumShips, mShipTransforms.data());
  BufferLayoutUnbind();
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

  BufferLayoutBind(mBufferShipSteering);
  BufferLayoutDataSet(mBufferShipSteering, mNumShips, mShipSteerings.data());
  BufferLayoutUnbind();
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

  BufferLayoutBind(mBufferShipPath);
  BufferLayoutDataSet(mBufferShipPath, mNumPaths * mNumPathsSub, mShipPaths.data());
  BufferLayoutUnbind();
}
void ShipManager::InitializeOctreeNodes()
{
  mOctreeNodes.resize(mNumOctreeLevels);

  BufferLayoutBind(mBufferShipOctree);
  BufferLayoutDataSet(mBufferShipOctree, mNumOctreeLevels, mOctreeNodes.data());
  BufferLayoutUnbind();
}