#pragma once

#include <Api.h>

#include <ShipDatabase.h>

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

struct ShipOctree
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

  Ship                            mShipScoutWespe            { ShipDatabaseGet("ScoutWespe") };

  UniformLayout<Steering>&        mUniformSteering           { RegistryGetOrCreate<UniformLayout<Steering>>("uniformSteering") };
  UniformLayout<Noise>&           mUniformNoise              { RegistryGetOrCreate<UniformLayout<Noise>>("uniformNoise") };

  std::vector<BufferTransform>    mShipTransforms            {};
  std::vector<ShipSteering>       mShipSteerings             {};
  std::vector<ShipWaypoint>       mShipPaths                 {};
  std::vector<ShipOctree>         mOctreeNodes               {};

  BufferLayout<BufferTransform>   mBufferShipTransform       {};
  BufferLayout<ShipSteering>      mBufferShipSteering        {};
  BufferLayout<ShipWaypoint>      mBufferShipPath            {};
  BufferLayout<ShipOctree>        mBufferShipOctree          {};

  ComputeMaterialDefault          mMaterialComputeShipPhysics{ mNumShips / 32 };
  ComputeMaterialDefault          mMaterialComputeShipPaths  { mNumPaths };
  ComputeMaterialDefault          mMaterialComputeShipOctree {};

  Context const&                  mContext                   { RegistryGetOrCreate<Context>("context") };
  Renderer&                       mRenderer                  { RegistryGetOrCreate<Renderer>("renderer") };
};