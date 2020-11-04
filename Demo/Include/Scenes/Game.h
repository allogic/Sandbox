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

struct ConfigSteering
{
  r32 mTimeDelta        {};
  r32 mAccelerationSpeed{};
  r32 mVelocityDecay    {};
  u32 mMaxPaths         {};
};
struct ConfigNoise
{
  r32v3 mRandomOffset{};
};
struct ConfigProjection
{
  r32m4 mProjection{};
  r32m4 mView      {};
  r32m4 mTransform {};
};

struct SceneGame : Scene
{
  u32                              mNumShips                  { 2048 * 32 };
  u32                              mNumPaths                  { 32 };
  u32                              mNumPathsSub               { 1024 };
  u32                              mNumPathsTotal             { mNumPaths * mNumPathsSub };
  u32                              mNumSkyDimension           { 512 };
  u32                              mNumMapDimension           { 512 };

  CameraControllerSpace            mCameraControllerSpace     {};
  CameraControllerOrbit            mCameraControllerOrbit     {};

  ModelLambert                     mModelShip                 {};
  ModelLambert                     mModelSky                  {};
  ModelLambert                     mModelMap                  {};

  TextureR32RGBA                   mTextureSky                {};
  TextureR32RGBA                   mTextureMap                {};

  std::vector<Transform>           mTransforms                {};
  std::vector<Steering>            mSteerings                 {};
  std::vector<Waypoint>            mPaths                     {};

  BufferLayout<Transform>          mBufferTransforms          {};
  BufferLayout<Steering>           mBufferSteerings           {};
  BufferLayout<Waypoint>           mBufferPaths               {};

  ConfigSteering                   mConfigSteering            {};
  ConfigNoise                      mConfigNoise               {};
  ConfigProjection                 mConfigProjection          {};

  UniformLayout<ConfigSteering>    mUniformConfigSteering     {};
  UniformLayout<ConfigNoise>       mUniformConfigNoise        {};
  UniformLayout<ConfigProjection>  mUniformConfigProjection   {};

  ShaderCompute                    mShaderComputeShipSteerings{};
  ShaderCompute                    mShaderComputeShipPhysics  {};
  ShaderCompute                    mShaderComputeShipPaths    {};
  ShaderCompute                    mShaderComputeSkyNoise     {};
  ShaderCompute                    mShaderComputeMapNoise     {};

  ShaderLambertInstanced           mShaderRenderShips         {};
  ShaderLambert                    mShaderRenderSky           {};
  ShaderLambert                    mShaderRenderMap           {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) override;
  void OnGizmos(r32 timeDelta) override;

  void CameraControllerUpdateInputSpace(r32 timeDelta);
  void CameraControllerUpdateInputOrbit(r32 timeDelta);

  void InitializeTransforms();
  void InitializeSteerings();
  void InitializePaths();
};