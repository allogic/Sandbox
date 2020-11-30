#pragma once

#include <Api.h>

#include <ModelDatabase.h>

/*
* Player actor.
*/

struct Player : Actor
{
  Context&               mContext   { RegistryGetOrCreate<Context>("context") };
  Renderer&              mRenderer  { RegistryGetOrCreate<Renderer>("renderer") };

  std::string            mModelName;

  Transform&             mTransform { ACS::Attach<Transform>(this) };
  Camera&                mCamera    { ACS::Attach<Camera>(this) };
  CameraControllerSpace& mController{ ACS::Attach<CameraControllerSpace>(this) };
  Renderable&            mRenderable{ ACS::Attach<Renderable>(this, &ModelDatabaseGet(mModelName).mMesh, &ModelDatabaseGet(mModelName).mTextureAlbedo, &ModelDatabaseGet(mModelName).mTextureSpecular, nullptr, nullptr, nullptr) };

  Player(Object* pObject, std::string const& modelName)
    : mModelName{ modelName }
    , Actor(pObject)
  {
    //mTransform.mRotation = { 0.f, 0.f, 45.f };

    mRenderable.mpMeshLambert->mTransform.mScale = { 0.2f, 0.2f, 0.2f };

    mCamera.mTransform.mPosition = { 0.f, 2.f, -10.f };
  }

  void OnUpdate(r32 timeDelta) override
  {
    if (KeyHeld(mContext, GLFW_KEY_A)) mController.mPositionAccel += glm::normalize(mCamera.mLocalRight) * mController.mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_D)) mController.mPositionAccel += -glm::normalize(mCamera.mLocalRight) * mController.mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_S)) mController.mPositionAccel += glm::normalize(mCamera.mLocalFront) * mController.mPositionAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_W)) mController.mPositionAccel += -glm::normalize(mCamera.mLocalFront) * mController.mPositionAmount * timeDelta;

    if (KeyHeld(mContext, GLFW_KEY_Q)) mController.mRotationAccel += glm::normalize(mCamera.mFront) * mController.mRotationAmount * timeDelta;
    if (KeyHeld(mContext, GLFW_KEY_E)) mController.mRotationAccel += -glm::normalize(mCamera.mFront) * mController.mRotationAmount * timeDelta;

    static r32v2 mousePositionDown{};

    if (MouseDown(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      mousePositionDown = MousePosition(mContext);
    }
    if (MouseHeld(mContext, GLFW_MOUSE_BUTTON_RIGHT))
    {
      r32v2 mousePosition{ MousePosition(mContext) };
      r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
      r32v3 mousePositionDeltaAligned{ -mousePositionDelta.y, -mousePositionDelta.x, 0.f };

      if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
        mController.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mController.mRotationAmount * timeDelta;
    }

    mController.mPositionAccel += -mController.mPositionAccel * mController.mPositionDecay * timeDelta;
    mController.mRotationAccel += -mController.mRotationAccel * mController.mRotationDecay * timeDelta;
  }
  void OnUpdateFixed(r32 timeDelta) override
  {
    mTransform.mPosition += mController.mPositionAccel;

    mTransform.mRotation += mController.mRotationAccel;
    if (mTransform.mRotation.x > 180.f) mTransform.mRotation.x = -180.f;
    if (mTransform.mRotation.y > 180.f) mTransform.mRotation.y = -180.f;
    if (mTransform.mRotation.z > 180.f) mTransform.mRotation.z = -180.f;

    if (mTransform.mRotation.x < -180.f) mTransform.mRotation.x = 180.f;
    if (mTransform.mRotation.y < -180.f) mTransform.mRotation.y = 180.f;
    if (mTransform.mRotation.z < -180.f) mTransform.mRotation.z = 180.f;

    static r32v3 rotationPrev{};

    r32v3 rotationDelta{ mTransform.mRotation - rotationPrev };

    rotationPrev = mTransform.mRotation;

    r32m4 localRotation = glm::identity<r32m4>();
    localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.x), mCamera.mLocalRight);
    localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.y), mCamera.mLocalUp);
    localRotation = glm::rotate(localRotation, glm::radians(rotationDelta.z), mCamera.mLocalFront);

    mCamera.mLocalRight = glm::normalize(localRotation * r32v4{ mCamera.mLocalRight, 1.f });
    mCamera.mLocalUp = glm::normalize(localRotation * r32v4{ mCamera.mLocalUp, 1.f });
    mCamera.mLocalFront = glm::normalize(localRotation * r32v4{ mCamera.mLocalFront, 1.f });
  }
  void OnGizmos(r32 timeDelta) override
  {
    // world
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f, 1.f });

    // local
    r32v3 cameraPosition{ mTransform.mPosition + mCamera.mTransform.mPosition };

    RendererLineBatchPushLine(mRenderer, cameraPosition, cameraPosition + mCamera.mLocalRight, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, cameraPosition, cameraPosition + mCamera.mLocalUp, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, cameraPosition, cameraPosition + mCamera.mLocalFront, { 0.f, 0.f, 1.f, 1.f });
  }
};