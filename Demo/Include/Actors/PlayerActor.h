#pragma once

#include <Api.h>

/*
* Player actor.
*/

struct Player : Actor
{
  Camera*                mpCamera    { ACS::Attach<Camera>(this) };
  CameraControllerSpace* mpController{ ACS::Attach<CameraControllerSpace>(this) };

  Player(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnUpdate(r32 time, r32 timeDelta) override
  {
    r32m4 localTransform{ TransformTo({ 0.f, 0.f, 0.f }, mpTransform->mRotation, { 1.f, 1.f, 1.f }) };

    r32v3 localRight = glm::normalize(localTransform * r32v4{ mpCamera->mRight, 1.f });
    r32v3 localUp = glm::normalize(localTransform * r32v4{ mpCamera->mUp, 1.f });
    r32v3 localFront = glm::normalize(localTransform * r32v4{ mpCamera->mFront, 1.f });

    if (KeyHeld(mWindow, GLFW_KEY_A)) mpController->mPositionAccel += -localRight * mpController->mPositionAmount * timeDelta;
    if (KeyHeld(mWindow, GLFW_KEY_D)) mpController->mPositionAccel += localRight * mpController->mPositionAmount * timeDelta;

    if (KeyHeld(mWindow, GLFW_KEY_S)) mpController->mPositionAccel += localFront * mpController->mPositionAmount * timeDelta;
    if (KeyHeld(mWindow, GLFW_KEY_W)) mpController->mPositionAccel += -localFront * mpController->mPositionAmount * timeDelta;

    if (KeyHeld(mWindow, GLFW_KEY_Q)) mpController->mRotationAccel += localFront * mpController->mRotationAmount * timeDelta;
    if (KeyHeld(mWindow, GLFW_KEY_E)) mpController->mRotationAccel += -localFront * mpController->mRotationAmount * timeDelta;

    static r32v2 mousePositionDown{};

    if (MouseDown(mWindow, GLFW_MOUSE_BUTTON_RIGHT))
    {
      mousePositionDown = MousePosition(mWindow);
    }
    if (MouseHeld(mWindow, GLFW_MOUSE_BUTTON_RIGHT))
    {
      r32v2 mousePosition{ MousePosition(mWindow) };
      r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
      r32v3 mousePositionDeltaAligned{ -mousePositionDelta.y, -mousePositionDelta.x, 0.f };

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
    mpTransform->mPosition += mpController->mPositionAccel;
    mpTransform->mRotation += mpController->mRotationAccel;

    if (mpTransform->mRotation.x > 180.f) mpTransform->mRotation.x = -180.f;
    if (mpTransform->mRotation.y > 180.f) mpTransform->mRotation.y = -180.f;
    if (mpTransform->mRotation.z > 180.f) mpTransform->mRotation.z = -180.f;
    
    if (mpTransform->mRotation.x < -180.f) mpTransform->mRotation.x = 180.f;
    if (mpTransform->mRotation.y < -180.f) mpTransform->mRotation.y = 180.f;
    if (mpTransform->mRotation.z < -180.f) mpTransform->mRotation.z = 180.f;
  }
};