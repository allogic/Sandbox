#pragma once

#include <Api.h>

/*
* Player controller actor.
*/

struct PlayerController : Actor
{
  Context&               mContext   { RegistryGetOrCreate<Context>("context") };
  Renderer&              mRenderer  { RegistryGetOrCreate<Renderer>("renderer") };

  Camera&                mCamera    { ACS::Attach<Camera>(this) };
  CameraControllerSpace& mController{ ACS::Attach<CameraControllerSpace>(this) };

  PlayerController(Object* pObject) : Actor(pObject) {}

  void OnUpdate(r32 timeDelta) override
  {
    r32m4 localTransform{ TransformTo({ 0.f, 0.f, 0.f }, mpParent->mTransform.mRotation, { 1.f, 1.f, 1.f }) };

    //r32v3 localRight = glm::normalize(localTransform * r32v4{ 1.f, 0.f, 0.f, 1.f });
    //r32v3 localUp = glm::normalize(localTransform * r32v4{ 0.f, 1.f, 0.f, 1.f });
    //r32v3 localFront = glm::normalize(localTransform * r32v4{ 0.f, 0.f, 1.f, 1.f });

    r32v3 localRight = glm::normalize(localTransform * r32v4{ 1.f, 0.f, 0.f, 1.f });
    r32v3 localUp = glm::normalize(localTransform * r32v4{ 0.f, 1.f, 0.f, 1.f });
    r32v3 localFront = glm::normalize(localTransform * r32v4{ 0.f, 0.f, 1.f, 1.f });

    if (KeyHeld(mContext, GLFW_KEY_A)) mController.mPositionAccel += -localRight * mController.mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_D)) mController.mPositionAccel += localRight * mController.mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_S)) mController.mPositionAccel += localFront * mController.mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_W)) mController.mPositionAccel += -localFront * mController.mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_Q)) mController.mRotationAccel += localFront * mController.mRotationAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_E)) mController.mRotationAccel += -localFront * mController.mRotationAmount * timeDelta;

    static r32v2 mousePositionDown{};

    if (MouseDown(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      mousePositionDown = MousePosition(mContext);
    }
    if (MouseHeld(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      r32v2 mousePosition{ MousePosition(mContext) };
      r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
      r32v3 mousePositionDeltaAligned{ mousePositionDelta.y, mousePositionDelta.x, 0.f };

      if (glm::length(mousePositionDelta) > glm::epsilon<r32>())
      {
        mController.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mController.mRotationAmount * timeDelta;
      }
    }

    mController.mPositionAccel += -mController.mPositionAccel * mController.mPositionDecay * timeDelta;
    mController.mRotationAccel += -mController.mRotationAccel * mController.mRotationDecay * timeDelta;
  }
  void OnUpdateFixed(r32 timeDelta) override
  {
    mpParent->mTransform.mPosition += mController.mPositionAccel;
    mpParent->mTransform.mRotation += mController.mRotationAccel;

    //if (mpParent->mTransform.mRotation.x > 180.f) mpParent->mTransform.mRotation.x = -180.f;
    //if (mpParent->mTransform.mRotation.y > 180.f) mpParent->mTransform.mRotation.y = -180.f;
    //if (mpParent->mTransform.mRotation.z > 180.f) mpParent->mTransform.mRotation.z = -180.f;
    //
    //if (mpParent->mTransform.mRotation.x < -180.f) mpParent->mTransform.mRotation.x = 180.f;
    //if (mpParent->mTransform.mRotation.y < -180.f) mpParent->mTransform.mRotation.y = 180.f;
    //if (mpParent->mTransform.mRotation.z < -180.f) mpParent->mTransform.mRotation.z = 180.f;
  }
};