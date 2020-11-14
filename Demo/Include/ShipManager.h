#pragma once

#include <Api.h>

struct ShipSteering
{
  r32v3 mAcceleration{};
  r32v3 mVelocity    {};
  u32   mPathIndex   {};
  u32   mPathIndexSub{};
};
struct ShipWaypoint
{
  r32v3 mPosition{};
};

struct OctreeNode
{

};

struct Steering
{
  r32 mTimeDelta        {};
  r32 mAccelerationSpeed{};
  r32 mVelocityDecay    {};
  u32 mMaxPaths         {};
};
struct Noise
{
  r32v3 mRandomOffset{};
  r32   mFrequency   {};
};

struct ShipManager
{
  ShipManager(
    u32 numShips,
    u32 numOctreeLevels,
    u32 numPaths,
    u32 numPathsSub);
  virtual ~ShipManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  void InitializeShipTransforms();
  void InitializeShipSteerings();
  void InitializeShipPaths();
  void InitializeOctreeNodes();

  u32 mNumShips;
  u32 mNumOctreeLevels;
  u32 mNumPaths;
  u32 mNumPathsSub;

  ModelLambert                    mModelShip                 {};

  UniformLayout<Steering>&        mUniformSteering           { RegistryGetOrCreate<UniformLayout<Steering>>("uniformSteering") };
  UniformLayout<Noise>&           mUniformNoise              { RegistryGetOrCreate<UniformLayout<Noise>>("uniformNoise") };

  std::vector<Transform>          mShipTransforms            {};
  std::vector<ShipSteering>       mShipSteerings             {};
  std::vector<ShipWaypoint>       mShipPaths                 {};
  std::vector<OctreeNode>         mOctreeNodes               {};

  BufferLayout<Transform>&        mBufferTransform           { RegistryGetOrCreate<BufferLayout<Transform>>("bufferTransform") };
  BufferLayout<ShipSteering>      mBufferSteering            {};
  BufferLayout<ShipWaypoint>      mBufferPath                {};
  BufferLayout<OctreeNode>        mBufferOctreeNode          {};

  ComputeMaterialDefault          mMaterialComputeShipPhysics{ mNumShips / 32, 1, 1 };
  ComputeMaterialDefault          mMaterialComputeShipPaths  { mNumPaths, 1, 1 };
  ComputeMaterialDefault          mMaterialComputeShipOctree { 1, 1, 1 };

  RenderMaterialLambertInstanced& mMaterialLambertInstanced  { RegistryGetOrCreate<RenderMaterialLambertInstanced>("materialLambertInstanced") };
};