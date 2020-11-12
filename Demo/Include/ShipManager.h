#pragma once

#include <Api.h>
#include <Object.h>

struct ShipTransform
{
  r32v3 mPosition          {};
  r32v3 mRotationLocalRight{};
  r32v3 mRotationLocalUp   {};
  r32v3 mRotationLocalFront{};
};
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

struct ShipManager : Object
{
  ShipManager(
    u32 numShips,
    u32 numOctreeLevels,
    u32 numPaths,
    u32 numPathsSub);
  virtual ~ShipManager();

  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) override;
  void OnGizmos(r32 timeDelta) override;

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

  std::vector<ShipTransform>      mShipTransforms            {};
  std::vector<ShipSteering>       mShipSteerings             {};
  std::vector<ShipWaypoint>       mShipPaths                 {};
  std::vector<OctreeNode>         mOctreeNodes               {};

  BufferLayout<ShipTransform>     mBufferTransforms          {};
  BufferLayout<ShipSteering>      mBufferSteerings           {};
  BufferLayout<ShipWaypoint>      mBufferPaths               {};
  BufferLayout<OctreeNode>        mBufferOctreeNodes         {};

  ComputeMaterialDefault          mMaterialComputeShipPhysics{};
  ComputeMaterialDefault          mMaterialComputeShipPaths  {};
  ComputeMaterialDefault          mMaterialComputeShipOctree {};

  RenderMaterialLambertInstanced& mMaterialLambertInstanced  { RegistryGetOrCreate<RenderMaterialLambertInstanced>("materialLambertInstanced") };
};