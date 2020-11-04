#pragma once

#include <Core.h>
#include <Types.h>

/*
* Camera layouts.
*/

struct Camera
{
  r32v3 mPosition  {};
  r32   mAspect    {};
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

/*
* Camera controller layouts.
*/

struct CameraControllerSpace
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
struct CameraControllerOrbit
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

/*
* Camera management.
*/

template<typename Camera> void CameraCreate(Camera& camera, r32v3 const& position, r32 aspect, r32 fov, r32 near, r32 far)
{
  r32 const fovRad{ glm::radians(fov) };
  r32v3 const right{ 1.f, 0.f, 0.f };
  r32v3 const up{ 0.f, 1.f, 0.f };
  r32v3 const front{ 0.f, 0.f, -1.f };

  camera =
  {
    .mPosition  { position },
    .mAspect    { aspect },
    .mFovRad    { fovRad },
    .mRight     { right },
    .mUp        { up },
    .mFront     { front },
    .mLocalRight{ right },
    .mLocalUp   { up },
    .mLocalFront{ front },
    .mNear      { near },
    .mFar       { far },
    .mProjection{ glm::perspective(fovRad, aspect, near, far) },
    .mView      { glm::lookAt(position, position + front, up) },
  };
}

/*
* Camera controller managmenet.
*/

template<typename Camera, typename Controller> void CameraControllerUpdatePhysicsSpace(Camera& camera, Controller& controller)
{
  controller.mPositionVelocity += controller.mPositionAccel;
  controller.mPositionAccel = {};
  camera.mPosition += controller.mPositionVelocity;

  controller.mRotationVelocityAngular += controller.mRotationAccel;
  controller.mRotationAccel = {};
  if (controller.mRotationVelocityAngular.x > 180.f) controller.mRotationVelocityAngular.x = -180.f;
  if (controller.mRotationVelocityAngular.x < -180.f) controller.mRotationVelocityAngular.x = 180.f;
  if (controller.mRotationVelocityAngular.y > 180.f) controller.mRotationVelocityAngular.y = -180.f;
  if (controller.mRotationVelocityAngular.y < -180.f) controller.mRotationVelocityAngular.y = 180.f;

  controller.mRollVelocityAngular += controller.mRollAccel;
  controller.mRollAccel = {};
  if (controller.mRollVelocityAngular > 180.f) controller.mRollVelocityAngular = -180.f;
  if (controller.mRollVelocityAngular < -180.f) controller.mRollVelocityAngular = 180.f;

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.y), camera.mLocalRight);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.x), camera.mLocalUp);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRollVelocityAngular), camera.mLocalFront);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };

  camera.mProjection = glm::perspective(camera.mFovRad, camera.mAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition, camera.mPosition + camera.mLocalFront, camera.mLocalUp);
}
template<typename Camera, typename Controller> void CameraControllerUpdatePhysicsOrbit(Camera& camera, Controller& controller)
{
  controller.mPositionVelocity += controller.mPositionAccel;
  controller.mPositionAccel = {};
  camera.mPosition += controller.mPositionVelocity;

  controller.mRotationVelocityAngular += controller.mRotationAccel;
  controller.mRotationAccel = {};
  if (controller.mRotationVelocityAngular.x > 180.f) controller.mRotationVelocityAngular.x = -180.f;
  if (controller.mRotationVelocityAngular.x < -180.f) controller.mRotationVelocityAngular.x = 180.f;
  if (controller.mRotationVelocityAngular.y > 180.f) controller.mRotationVelocityAngular.y = -180.f;
  if (controller.mRotationVelocityAngular.y < -180.f) controller.mRotationVelocityAngular.y = 180.f;

  controller.mScrollVelocity += controller.mScrollAccel;
  controller.mScrollAccel = {};
  controller.mScrollDistance += controller.mScrollVelocity;
  controller.mScrollDistance = glm::clamp(controller.mScrollDistance, controller.mScrollDistanceMin, controller.mScrollDistanceMax);

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.x), camera.mUp);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.y), camera.mLocalRight);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };

  camera.mProjection = glm::perspective(camera.mFovRad, camera.mAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition - camera.mLocalFront * controller.mScrollDistance, camera.mPosition, camera.mLocalUp);
}