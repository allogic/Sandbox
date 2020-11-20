#include <PlayerManager.h>

PlayerManager::PlayerManager()
{
  ModelCreate(mModelCruiser, SANDBOX_ENGINE_ROOT_PATH "Model\\CruiserBerlin.fbx");

  TextureLayoutCreate(mTextureCruiser, 512, 512);
  TextureLayoutBind(mTextureCruiser);
  TextureLayoutDataSetFrom(mTextureCruiser, SANDBOX_ENGINE_ROOT_PATH "Texture\\UV.png");
}
PlayerManager::~PlayerManager()
{
  ModelDestroy(mModelCruiser);
  TextureLayoutDestroy(mTextureCruiser);
}

void PlayerManager::Update(r32 timeDelta)
{
  CameraControllerUpdateInputSpace(mContext, mCamera, mCameraControllerSpace, timeDelta);
  //CameraControllerUpdateInputOrbit(mContext, mCamera, mCameraControllerOrbit, timeDelta);
}
void PlayerManager::UpdatePhysics(r32 timeDelta)
{
  CameraControllerUpdatePhysicsSpace(mCamera, mCameraControllerSpace);
  //CameraControllerUpdatePhysicsOrbit(mCamera, mCameraControllerOrbit);
}
void PlayerManager::Render()
{
  for (u32 i{}; i < 16; i++)
    for (u32 j{}; j < 16; j++)
    {
      RendererSubmitLambert(mRenderer, TaskLambert
      {
        &mModelCruiser,
        TransformTo({ ((r32)i - 8) * 30, 0.f, ((r32)j - 8) * 130 }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }),
        &mTextureCruiser,
      });
    }
}
void PlayerManager::Debug()
{
  for (u32 i{}; i < 2; i++)
  {
    r32v3 lightPosition{ mRenderer.mUniformBlockPointLights[i].mPosition };
    r32 lightRadius{ mRenderer.mUniformBlockPointLights[i].mRadius };

    RendererLineBatchPushLine(mRenderer, lightPosition, { lightPosition.x, 0.f, lightPosition.z }, { 1.f, 1.f, 1.f, 1.f });
    RendererLineBatchPushBox(mRenderer, lightPosition, { lightRadius * 2.f, lightRadius * 2.f, lightRadius * 2.f }, { 1.f, 1.f, 1.f, 1.f });
  }

  return;

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