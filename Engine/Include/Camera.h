#pragma once

#include <Core.h>
#include <Types.h>
#include <Events.h>

/*
* Camera layouts.
*/

struct Camera
{
  r32v3 mPosition  { 0.f, 30.f, 0.f };
  r32   mAspect    { 1280.f / 720.f };
  r32   mFovRad    { glm::radians(45.f) };
  r32v3 mRotation  {};
  r32v3 mRight     { 1.f, 0.f, 0.f };
  r32v3 mUp        { 0.f, 1.f, 0.f };
  r32v3 mFront     { 0.f, 0.f, 1.f };
  r32v3 mLocalRight{ mRight };
  r32v3 mLocalUp   { mUp };
  r32v3 mLocalFront{ mFront };
  r32   mNear      { 0.01f };
  r32   mFar       { 10000.f };
  r32m4 mProjection{};
  r32m4 mView      {};
};

/*
* Camera controller layouts.
*/

struct CameraControllerSpace
{
  r32   mPositionAmount         { 0.2f };
  r32   mRotationAmount         { 5.f };
  r32   mRollAmount             { 5.f };
  r32   mPositionVelocityDecay  { 0.4f };
  r32   mRotationVelocityDecay  { 0.5f };
  r32   mRollVelocityDecay      { 0.5f };
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
  r32   mRotationAmount         { 0.2f };
  r32   mScrollAmount           { 10.f };
  r32   mPositionVelocityDecay  { 20.f };
  r32   mRotationVelocityDecay  { 0.1f };
  r32   mScrollVelocityDecay    { 10.f };
  r32v3 mPositionVelocity       {};
  r32v2 mRotationVelocityAngular{};
  r32   mScrollVelocity         {};
  r32v3 mPositionAccel          {};
  r32v2 mRotationAccel          {};
  r32   mScrollAccel            {};
  r32   mScrollDistance         { 10.f };
  r32   mScrollDistanceMin      { 300.f };
  r32   mScrollDistanceMax      { 1000.f };
};

/*
* Camera physic management.
*/

template<typename Context, typename Camera, typename Controller> void CameraControllerUpdateInputSpace(Context const& context, Camera& camera, Controller& controller, r32 timeDelta)
{
  if (KeyHeld(context, GLFW_KEY_A)) controller.mPositionAccel += glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_D)) controller.mPositionAccel += -glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;

  if (KeyHeld(context, GLFW_KEY_S)) controller.mPositionAccel += -glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_W)) controller.mPositionAccel += glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;

  if (KeyHeld(context, GLFW_KEY_Q)) controller.mRollAccel += -controller.mRollAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_E)) controller.mRollAccel += controller.mRollAmount * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = MousePosition(context);
  }
  if (MouseHeld(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePosition(context) };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, -mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      controller.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * controller.mRotationAmount * timeDelta;
  }

  controller.mPositionVelocity += -controller.mPositionVelocity * controller.mPositionVelocityDecay * timeDelta;
  controller.mRotationVelocityAngular += -controller.mRotationVelocityAngular * controller.mRotationVelocityDecay * timeDelta;
  controller.mRollVelocityAngular += -controller.mRollVelocityAngular * controller.mRollVelocityDecay * timeDelta;
}
template<typename Context, typename Camera, typename Controller> void CameraControllerUpdateInputOrbit(Context const& context, Camera& camera, Controller& controller, r32 timeDelta)
{
  r32 distanceScaleLog{ glm::log(controller.mScrollDistance) };

  if (KeyHeld(context, GLFW_KEY_A)) controller.mPositionAccel += glm::normalize(camera.mLocalRight) * controller.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(context, GLFW_KEY_D)) controller.mPositionAccel += -glm::normalize(camera.mLocalRight) * controller.mPositionAmount * distanceScaleLog * timeDelta;

  r32v3 localFrontNoXRot{ glm::cross(camera.mUp, camera.mLocalRight) };

  if (KeyHeld(context, GLFW_KEY_S)) controller.mPositionAccel += glm::normalize(localFrontNoXRot) * controller.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(context, GLFW_KEY_W)) controller.mPositionAccel += -glm::normalize(localFrontNoXRot) * controller.mPositionAmount * distanceScaleLog * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = MousePosition(context);
  }
  if (MouseHeld(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePosition(context) };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, -mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      controller.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * controller.mRotationAmount * distanceScaleLog * timeDelta;
  }

  controller.mScrollAccel += -MouseScroll(context).y * controller.mScrollAmount * timeDelta;

  controller.mPositionVelocity += -controller.mPositionVelocity * controller.mPositionVelocityDecay * timeDelta;
  controller.mRotationVelocityAngular += -controller.mRotationVelocityAngular * controller.mRotationVelocityDecay * timeDelta;
  controller.mScrollVelocity += -controller.mScrollVelocity * controller.mScrollVelocityDecay * timeDelta;

  controller.mScrollVelocity = glm::clamp(controller.mScrollVelocity, -2.f, 2.f);
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
  camera.mRotation += r32v3{ controller.mRotationVelocityAngular.y, controller.mRotationVelocityAngular.x, 0.f };

  controller.mRollVelocityAngular += controller.mRollAccel;
  controller.mRollAccel = {};
  if (controller.mRollVelocityAngular > 180.f) controller.mRollVelocityAngular = -180.f;
  if (controller.mRollVelocityAngular < -180.f) controller.mRollVelocityAngular = 180.f;
  camera.mRotation += r32v3{ 0.f, 0.f, controller.mRollVelocityAngular };

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
  camera.mRotation += r32v3{ controller.mRotationVelocityAngular.y, controller.mRotationVelocityAngular.x, 0.f };
  if (camera.mRotation.x > 180.f) camera.mRotation.x = -180.f;
  if (camera.mRotation.x < -180.f) camera.mRotation.x = 180.f;
  if (camera.mRotation.y > 180.f) camera.mRotation.y = -180.f;
  if (camera.mRotation.y < -180.f) camera.mRotation.y = 180.f;

  controller.mScrollVelocity += controller.mScrollAccel;
  controller.mScrollAccel = {};
  controller.mScrollDistance += controller.mScrollVelocity;
  controller.mScrollDistance = glm::clamp(controller.mScrollDistance, controller.mScrollDistanceMin, controller.mScrollDistanceMax);

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.y), camera.mLocalRight);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.x), camera.mUp);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };

  camera.mProjection = glm::perspective(camera.mFovRad, camera.mAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition - camera.mLocalFront * controller.mScrollDistance, camera.mPosition, camera.mLocalUp);
}