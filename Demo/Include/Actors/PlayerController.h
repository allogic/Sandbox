#pragma once

#include <Api.h>

/*
* Player controller actor.
*/

struct PlayerController : Actor
{
  Context&               mContext    { RegistryGetOrCreate<Context>("context") };
  Renderer&              mRenderer   { RegistryGetOrCreate<Renderer>("renderer") };

  Camera*                mpCamera    { ACS::Attach<Camera>(this) };
  CameraControllerSpace* mpController{ ACS::Attach<CameraControllerSpace>(this) };

  PlayerController(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnUpdate(r32 time, r32 timeDelta) override
  {
    r32m4 localTransform{ TransformTo({ 0.f, 0.f, 0.f }, mpParent->mTransform.mRotation, { 1.f, 1.f, 1.f }) };

    //r32v3 localRight = glm::normalize(localTransform * r32v4{ 1.f, 0.f, 0.f, 1.f });
    //r32v3 localUp = glm::normalize(localTransform * r32v4{ 0.f, 1.f, 0.f, 1.f });
    //r32v3 localFront = glm::normalize(localTransform * r32v4{ 0.f, 0.f, 1.f, 1.f });

    r32v3 localRight = glm::normalize(localTransform * r32v4{ 1.f, 0.f, 0.f, 1.f });
    r32v3 localUp = glm::normalize(localTransform * r32v4{ 0.f, 1.f, 0.f, 1.f });
    r32v3 localFront = glm::normalize(localTransform * r32v4{ 0.f, 0.f, 1.f, 1.f });

    if (KeyHeld(mContext, GLFW_KEY_A)) mpController->mPositionAccel += -localRight * mpController->mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_D)) mpController->mPositionAccel += localRight * mpController->mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_S)) mpController->mPositionAccel += localFront * mpController->mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_W)) mpController->mPositionAccel += -localFront * mpController->mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_Q)) mpController->mRotationAccel += localFront * mpController->mRotationAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_E)) mpController->mRotationAccel += -localFront * mpController->mRotationAmount * timeDelta;

    static r32v2 mousePositionDown{};

    if (MouseDown(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      mousePositionDown = MousePosition(mContext);
    }
    if (MouseHeld(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      r32v2 mousePosition{ MousePosition(mContext) };
      r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
      r32v3 mousePositionDeltaAligned{ localTransform * r32v4{ mousePositionDelta.y, mousePositionDelta.x, 0.f, 1.f } };

      if (glm::length(mousePositionDelta) > glm::epsilon<r32>())
      {
        mpController->mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mpController->mRotationAmount * timeDelta;
      }
    }

    mpController->mPositionAccel += -mpController->mPositionAccel * mpController->mPositionDecay * timeDelta;
    mpController->mRotationAccel += -mpController->mRotationAccel * mpController->mRotationDecay * timeDelta;
  }
  void OnUpdateFixed(r32 time, r32 timeDelta) override
  {
    mpParent->mTransform.mPosition += mpController->mPositionAccel;
    mpParent->mTransform.mRotation += mpController->mRotationAccel;

    //if (mpParent->mTransform.mRotation.x > 180.f) mpParent->mTransform.mRotation.x = -180.f;
    //if (mpParent->mTransform.mRotation.y > 180.f) mpParent->mTransform.mRotation.y = -180.f;
    //if (mpParent->mTransform.mRotation.z > 180.f) mpParent->mTransform.mRotation.z = -180.f;
    //
    //if (mpParent->mTransform.mRotation.x < -180.f) mpParent->mTransform.mRotation.x = 180.f;
    //if (mpParent->mTransform.mRotation.y < -180.f) mpParent->mTransform.mRotation.y = 180.f;
    //if (mpParent->mTransform.mRotation.z < -180.f) mpParent->mTransform.mRotation.z = 180.f;
  }
};