#pragma once

#include <Api.h>

struct Transform
{
  r32v3 mPosition{};
  r32v3 mRotationLocalRight{};
  r32v3 mRotationLocalUp{};
  r32v3 mRotationLocalFront{};
};
struct Steering
{
  r32v3 mAcceleration{};
  r32v3 mVelocity    {};
  u32   mPathIndex   {};
  u32   mPathIndexSub{};
};
struct Waypoint
{
  r32v3 mPosition {};
  r32v3 mDirection{};
  r32v3 mRotationLocalFront{};
};

struct SceneGame : Scene
{
  u32                     mNumShips                  { 2048 * 32 };
  u32                     mNumPaths                  { 32 };
  u32                     mNumPathSub                { 1024 };

  CameraControllerOrbit   mCameraController          {};

  ModelLambert            mModelShip                 {};
  ModelLambert            mModelSky                  {};
  ModelLambert            mModelMap                  {};

  TextureR32RGBA          mTextureSky                {};
  TextureR32RGBA          mTextureMap                {};

  std::vector<Transform>  mTransforms                {};
  std::vector<Steering>   mSteerings                 {};
  std::vector<Waypoint>   mPaths                     {};

  BufferLayout<Transform> mBufferTransforms          {};
  BufferLayout<Steering>  mBufferSteerings           {};
  BufferLayout<Waypoint>  mBufferPaths               {};

  ShaderCompute           mShaderComputeShipSteerings{};
  ShaderCompute           mShaderComputeShipPhysics  {};
  ShaderCompute           mShaderComputeShipPaths    {};
  ShaderCompute           mShaderComputeMapNoise     {};

  ShaderShip              mShaderRenderShips         {};
  ShaderLambert           mShaderRenderSky           {};
  ShaderLambert           mShaderRenderMap           {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;

  void InitializeTransforms();
  void InitializeSteerings();
  void InitializePaths();
};