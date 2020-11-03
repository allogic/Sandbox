#pragma once

#include <Types.h>
#include <Core.h>

struct SANDBOX_ENGINE_API Camera
{
  r32v3 mPosition  {};
  r32   mFovRad    {};
  r32v3 mRight     {};
  r32v3 mUp        {};
  r32v3 mFront     {};
  r32v3 mLocalRight{};
  r32v3 mLocalUp   {};
  r32v3 mLocalFront{};
  r32   mNear      {};
  r32   mFar       {};
  r32m4 mProjection{};
  r32m4 mView      {};
};
struct SANDBOX_ENGINE_API CameraControllerSpace
{
  r32   mPositionAmount         { 10.f };
  r32   mRotationAmount         { 40.f };
  r32   mRollAmount             { 40.f };
  r32   mPositionVelocityDecay  { 20.f };
  r32   mRotationVelocityDecay  { 30.f };
  r32   mRollVelocityDecay      { 30.f };
  r32v3 mPositionVelocity       {};
  r32v2 mRotationVelocityAngular{};
  r32   mRollVelocityAngular    {};
  r32v3 mPositionAccel          {};
  r32v2 mRotationAccel          {};
  r32   mRollAccel              {};
};
struct SANDBOX_ENGINE_API CameraControllerOrbit
{
  r32   mPositionAmount         { 10.f };
  r32   mRotationAmount         { 25.f };
  r32   mScrollAmount           { 1000.f };
  r32   mPositionVelocityDecay  { 20.f };
  r32   mRotationVelocityDecay  { 20.f };
  r32   mScrollVelocityDecay    { 10.f };
  r32v3 mPositionVelocity       {};
  r32v2 mRotationVelocityAngular{};
  r32   mScrollVelocity         {};
  r32v3 mPositionAccel          {};
  r32v2 mRotationAccel          {};
  r32   mScrollAccel            {};
  r32   mScrollDistance         { 100.f };
  r32   mScrollDistanceMin      { 10.f };
  r32   mScrollDistanceMax      { 1000.f };
};
struct Scene
{
  Camera mCamera{};

  virtual ~Scene() = default;

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
  virtual void OnUpdate(r32 timeDelta) = 0;
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnRender(r32 timeDelta) const = 0;
  virtual void OnGizmos(r32 timeDelta) {}
};