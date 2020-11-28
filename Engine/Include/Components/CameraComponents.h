#pragma once

#include <Core.h>
#include <Types.h>
#include <Events.h>
#include <ACS.h>

/*
* Camera components.
*/

struct Camera : Component
{
  r32   mAspect       { 1280.f / 720.f };
  r32   mFovRad       { glm::radians(45.f) };
  r32v3 mRight        { 1.f, 0.f, 0.f };
  r32v3 mUp           { 0.f, 1.f, 0.f };
  r32v3 mFront        { 0.f, 0.f, 1.f };
  r32v3 mLocalRight   { mRight };
  r32v3 mLocalUp      { mUp };
  r32v3 mLocalFront   { mFront };
  r32   mNear         { 0.1f };
  r32   mFar          { 100000.f };
  r32m4 mProjection   {};
  r32m4 mView         {};
  r32m4 mTransform    {};
};

/*
* Camera controller components.
*/

struct CameraControllerSpace : Component
{
  r32   mPositionAmount{ 5.f };
  r32   mRotationAmount{ 5.f };
  r32   mPositionDecay { 0.4f };
  r32   mRotationDecay { 1.5f };
  r32v3 mPositionAccel {};
  r32v3 mRotationAccel {};
};
struct CameraControllerOrbit : Component
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
* Camera controller managmenet.
*/

template<typename Context, typename Camera, typename Controller> void CameraControllerUpdateInputSpace(Context const& context, Camera& camera, Controller& controller, r32 timeDelta)
{
  if (KeyHeld(context, GLFW_KEY_A)) controller.mPositionAccel += glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_D)) controller.mPositionAccel += -glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;

  if (KeyHeld(context, GLFW_KEY_S)) controller.mPositionAccel += -glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_W)) controller.mPositionAccel += glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;

  if (KeyHeld(context, GLFW_KEY_Q)) controller.mRotationAccel += -glm::normalize(camera.mFront) * controller.mRotationAmount * timeDelta;
  if (KeyHeld(context, GLFW_KEY_E)) controller.mRotationAccel += glm::normalize(camera.mFront) * controller.mRotationAmount * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = MousePosition(context);
  }
  if (MouseHeld(context, GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePosition(context) };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v3 mousePositionDeltaAligned{ mousePositionDelta.x, -mousePositionDelta.y, 0.f };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      controller.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * controller.mRotationAmount * timeDelta;
  }

  controller.mPositionAccel += -controller.mPositionAccel * controller.mPositionDecay * timeDelta;
  controller.mRotationAccel += -controller.mRotationAccel * controller.mRotationDecay * timeDelta;
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
* Camera physic management.
*/

template<typename Transform, typename Camera, typename Controller> void CameraControllerUpdatePhysicsSpace(Transform& transform, Camera& camera, Controller& controller)
{
  transform.mPosition += controller.mPositionAccel;

  transform.mRotation += controller.mRotationAccel;
  if (transform.mRotation.x > 180.f) transform.mRotation.x = -180.f;
  if (transform.mRotation.x < -180.f) transform.mRotation.x = 180.f;
  if (transform.mRotation.y > 180.f) transform.mRotation.y = -180.f;
  if (transform.mRotation.y < -180.f) transform.mRotation.y = 180.f;
  if (transform.mRotation.z > 180.f) transform.mRotation.z = -180.f;
  if (transform.mRotation.z < -180.f) transform.mRotation.z = 180.f;

  static r32v3 rotationPrev{};

  r32v3 rotationDelta = transform.mRotation - rotationPrev;
  rotationPrev = transform.mRotation;

  r32m4 localRotation = glm::identity<r32m4>();
  localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.y), camera.mLocalRight);
  localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.x), camera.mLocalUp);
  localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.z), camera.mLocalFront);

  camera.mLocalRight = glm::normalize(localRotation * r32v4{ camera.mLocalRight, 1.f });
  camera.mLocalUp = glm::normalize(localRotation * r32v4{ camera.mLocalUp, 1.f });
  camera.mLocalFront = glm::normalize(localRotation * r32v4{ camera.mLocalFront, 1.f });

  camera.mTransform = glm::translate(glm::identity<r32m4>(), transform.mPosition);
  camera.mProjection = glm::perspective(camera.mFovRad, camera.mAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt({}, camera.mLocalFront, camera.mLocalUp);
}
template<typename Transform, typename Camera, typename Controller> void CameraControllerUpdatePhysicsOrbit(Transform& transform, Camera& camera, Controller& controller)
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