#include <PlayerManager.h>

PlayerManager::PlayerManager()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, WindowAspect(), 45.f, 0.001f, 10000.f);
}
PlayerManager::~PlayerManager()
{

}

void PlayerManager::OnUpdate(r32 timeDelta)
{
  //CameraControllerUpdateInputSpace(timeDelta);
  CameraControllerUpdateInputOrbit(timeDelta);
}
void PlayerManager::OnUpdateFixed(r32 timeDelta)
{
  //CameraControllerUpdatePhysicsSpace(mCamera, mCameraControllerSpace);
  CameraControllerUpdatePhysicsOrbit(mCamera, mCameraControllerOrbit);

  UniformLayoutBind(mUniformProjection);
  Projection projection{};
  UniformLayoutDataGet(mUniformProjection, 1, &projection);
  projection.mProjection = mCamera.mProjection;
  projection.mView = mCamera.mView;
  UniformLayoutDataSet(mUniformProjection, 1, &projection);
}
void PlayerManager::OnRender(r32 timeDelta)
{

}
void PlayerManager::OnGizmos(r32 timeDelta)
{

}

void PlayerManager::CameraControllerUpdateInputSpace(r32 timeDelta)
{
  if (KeyHeld(GLFW_KEY_A)) mCameraControllerSpace.mPositionAccel += -glm::normalize(mCamera.mLocalRight) * mCameraControllerSpace.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) mCameraControllerSpace.mPositionAccel += glm::normalize(mCamera.mLocalRight) * mCameraControllerSpace.mPositionAmount * timeDelta;

  if (KeyHeld(GLFW_KEY_S)) mCameraControllerSpace.mPositionAccel += -glm::normalize(mCamera.mLocalFront) * mCameraControllerSpace.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) mCameraControllerSpace.mPositionAccel += glm::normalize(mCamera.mLocalFront) * mCameraControllerSpace.mPositionAmount * timeDelta;

  if (KeyHeld(GLFW_KEY_Q)) mCameraControllerSpace.mRollAccel += -mCameraControllerSpace.mRollAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_E)) mCameraControllerSpace.mRollAccel += mCameraControllerSpace.mRollAmount * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      mCameraControllerSpace.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mCameraControllerSpace.mRotationAmount * timeDelta;
  }

  mCameraControllerSpace.mPositionVelocity += -mCameraControllerSpace.mPositionVelocity * mCameraControllerSpace.mPositionVelocityDecay * timeDelta;
  mCameraControllerSpace.mRotationVelocityAngular += -mCameraControllerSpace.mRotationVelocityAngular * mCameraControllerSpace.mRotationVelocityDecay * timeDelta;
  mCameraControllerSpace.mRollVelocityAngular += -mCameraControllerSpace.mRollVelocityAngular * mCameraControllerSpace.mRollVelocityDecay * timeDelta;
}
void PlayerManager::CameraControllerUpdateInputOrbit(r32 timeDelta)
{
  r32 distanceScaleLog{ glm::log(mCameraControllerOrbit.mScrollDistance) };

  if (KeyHeld(GLFW_KEY_A)) mCameraControllerOrbit.mPositionAccel += -glm::normalize(mCamera.mLocalRight) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) mCameraControllerOrbit.mPositionAccel += glm::normalize(mCamera.mLocalRight) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;

  r32v3 mLocalFrontNoXRot{ glm::cross(mCamera.mUp, mCamera.mLocalRight) };

  if (KeyHeld(GLFW_KEY_S)) mCameraControllerOrbit.mPositionAccel += -glm::normalize(mLocalFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) mCameraControllerOrbit.mPositionAccel += glm::normalize(mLocalFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      mCameraControllerOrbit.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * mCameraControllerOrbit.mRotationAmount * distanceScaleLog * timeDelta;
  }

  mCameraControllerOrbit.mScrollAccel += -MouseScrollY() * mCameraControllerOrbit.mScrollAmount * timeDelta;

  mCameraControllerOrbit.mPositionVelocity += -mCameraControllerOrbit.mPositionVelocity * mCameraControllerOrbit.mPositionVelocityDecay * timeDelta;
  mCameraControllerOrbit.mRotationVelocityAngular += -mCameraControllerOrbit.mRotationVelocityAngular * mCameraControllerOrbit.mRotationVelocityDecay * timeDelta;
  mCameraControllerOrbit.mScrollVelocity += -mCameraControllerOrbit.mScrollVelocity * mCameraControllerOrbit.mScrollVelocityDecay * timeDelta;

  mCameraControllerOrbit.mScrollVelocity = glm::clamp(mCameraControllerOrbit.mScrollVelocity, -2.f, 2.f);
}