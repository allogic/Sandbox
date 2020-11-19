#include <PlayerManager.h>

PlayerManager::PlayerManager()
{
  ModelCreate(mModelCruiser, SANDBOX_ENGINE_ROOT_PATH "Model\\CruiserBerlin.fbx");
  ModelLayoutTransform(mModelCruiser, { 10.f, 10.f, 10.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });

  TextureLayoutCreate(mTextureCruiser, 512, 512);
  TextureLayoutBind(mTextureCruiser);
  TextureLayoutDataSetFrom(mTextureCruiser, SANDBOX_ENGINE_ROOT_PATH "Texture\\UV.png");

  CameraCreate(mCamera, { 0.f, 20.f, 0.f }, WindowAspect(), 45.f, 0.001f, 10000.f);
}
PlayerManager::~PlayerManager()
{
  ModelDestroy(mModelCruiser);
  TextureLayoutDestroy(mTextureCruiser);
}

void PlayerManager::Update(r32 timeDelta)
{
  CameraControllerUpdateInputSpace(timeDelta);
  //CameraControllerUpdateInputOrbit(timeDelta);
}
void PlayerManager::UpdatePhysics(r32 timeDelta)
{
  CameraControllerUpdatePhysicsSpace(mCamera, mCameraControllerSpace);
  //CameraControllerUpdatePhysicsOrbit(mCamera, mCameraControllerOrbit);
}
void PlayerManager::Render()
{
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mModelCruiser,
    &mTextureCruiser,
  });
}
void PlayerManager::Debug()
{
  u32 size{ 32 };
  
  for (u32 i{}; i <= size; i++)
    for (u32 j{}; j <= size; j++)
    {
      r32v3 spacing{ 10.f, 10.f, 10.f };
      r32v3 half{ size / 2, 0.f, size / 2 };
      r32v3 startP0{ i, 0, 0 };
      r32v3 startP1{ 0, 0, j };
      r32v3 endP0{ i, 0, size };
      r32v3 endP1{ size, 0, j };
      r32v4 color{ 1.f, 1.f, 0.f, 1.f };
  
      startP0 *= spacing;
      startP1 *= spacing;
      startP0 += -half * spacing;
      startP1 += -half * spacing;
  
      endP0 *= spacing;
      endP1 *= spacing;
      endP0 += -half * spacing;
      endP1 += -half * spacing;
  
      RendererLineBatchPushLine(mRenderer, startP0, endP0, color);
      RendererLineBatchPushLine(mRenderer, startP1, endP1, color);
    }
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

  r32v3 localFrontNoXRot{ glm::cross(mCamera.mUp, mCamera.mLocalRight) };

  if (KeyHeld(GLFW_KEY_S)) mCameraControllerOrbit.mPositionAccel += -glm::normalize(localFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) mCameraControllerOrbit.mPositionAccel += glm::normalize(localFrontNoXRot) * mCameraControllerOrbit.mPositionAmount * distanceScaleLog * timeDelta;

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