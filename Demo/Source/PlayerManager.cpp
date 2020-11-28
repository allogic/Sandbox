#include <PlayerManager.h>

PlayerManager::PlayerManager()
{
  MeshFrom(mMeshRenderTexture, SANDBOX_ROOT_PATH "Models\\Primitives\\Quad.obj");
}
PlayerManager::~PlayerManager()
{
  
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
  r32v3 offset{};
  u32 width{};
  u32 height{};
  u32 depth{};
  r32v3 spacing{};

  offset = { 0.f, 0.f, 0.f };
  width = 8;
  height = 8;
  depth = 1;
  spacing = { 32.f, 32.f, 128.f };
  for (u32 i{}; i < width; i++)
    for (u32 j{}; j < height; j++)
      for (u32 k{}; k < depth; k++)
      {
        RendererSubmitLambert(mRenderer, TaskLambert
        {
          &mShipCruiserBerlin.mMesh,
          TransformTo(offset + r32v3{ (r32)i - (r32)width / 2.f, (r32)j - (r32)height / 2.f, (r32)k - (r32)depth / 2.f } * spacing, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }),
          &mShipCruiserBerlin.mTextureAlbedo,
          &mShipCruiserBerlin.mTextureNormal,
          &mShipCruiserBerlin.mTextureSpecular,
          nullptr,
          nullptr,
        });
      }

  offset = { 0.f, 0.f, 50.f };
  width = 8;
  height = 8;
  depth = 1;
  spacing = { 16.f, 16.f, 16.f };
  for (u32 i{}; i < width; i++)
    for (u32 j{}; j < height; j++)
      for (u32 k{}; k < depth; k++)
      {
        RendererSubmitLambert(mRenderer, TaskLambert
        {
          &mShipFighterAtlas.mMesh,
          TransformTo(offset + r32v3{ (r32)i - (r32)width / 2.f, (r32)j - (r32)height / 2.f, (r32)k - (r32)depth / 2.f } * spacing, { 0.f, 0.f, 0.f }, { 4.f, 4.f, 4.f }),
          &mShipFighterAtlas.mTextureAlbedo,
          &mShipFighterAtlas.mTextureNormal,
          &mShipFighterAtlas.mTextureSpecular,
          nullptr,
          nullptr,
        });
      }

  offset = { 0.f, 0.f, 70.f };
  width = 16;
  height = 16;
  depth = 1;
  spacing = { 10.f, 10.f, 10.f };
  for (u32 i{}; i < width; i++)
    for (u32 j{}; j < height; j++)
      for (u32 k{}; k < depth; k++)
      {
        RendererSubmitLambert(mRenderer, TaskLambert
        {
          &mShipFighterLindwurm.mMesh,
          TransformTo(offset + r32v3{ (r32)i - (r32)width / 2.f, (r32)j - (r32)height / 2.f, (r32)k - (r32)depth / 2.f } * spacing, { 0.f, 0.f, 0.f }, { 3.f, 3.f, 3.f }),
          &mShipFighterLindwurm.mTextureAlbedo,
          &mShipFighterLindwurm.mTextureNormal,
          &mShipFighterLindwurm.mTextureSpecular,
          nullptr,
          nullptr,
        });
      }

  offset = { 0.f, 0.f, 90.f };
  width = 16;
  height = 16;
  depth = 1;
  spacing = { 10.f, 10.f, 10.f };
  for (u32 i{}; i < width; i++)
    for (u32 j{}; j < height; j++)
      for (u32 k{}; k < depth; k++)
      {
        RendererSubmitLambert(mRenderer, TaskLambert
        {
          &mShipScoutAdler.mMesh,
          TransformTo(offset + r32v3{ (r32)i - (r32)width / 2.f, (r32)j - (r32)height / 2.f, (r32)k - (r32)depth / 2.f } * spacing, { 0.f, 0.f, 0.f }, { 2.f, 2.f, 2.f }),
          &mShipScoutAdler.mTextureAlbedo,
          &mShipScoutAdler.mTextureNormal,
          &mShipScoutAdler.mTextureSpecular,
          nullptr,
          nullptr,
        });
      }

  offset = { 0.f, 0.f, 100.f };
  width = 8;
  height = 8;
  depth = 1;
  spacing = { 10.f, 10.f, 10.f };
  for (u32 i{}; i < width; i++)
    for (u32 j{}; j < height; j++)
      for (u32 k{}; k < depth; k++)
      {
        RendererSubmitLambert(mRenderer, TaskLambert
        {
          &mShipScoutWespe.mMesh,
          TransformTo(offset + r32v3{ (r32)i - (r32)width / 2.f, (r32)j - (r32)height / 2.f, (r32)k - (r32)depth / 2.f } * spacing, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }),
          &mShipScoutWespe.mTextureAlbedo,
          &mShipScoutWespe.mTextureNormal,
          &mShipScoutWespe.mTextureSpecular,
          nullptr,
          nullptr,
        });
      }

  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 0.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTexturePosition,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 20.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureAlbedo,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 40.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureNormal,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 60.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureSpecular,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 80.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureMetallic,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 100.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureRoughness,
  });
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshRenderTexture,
    TransformTo({ 120.f, 0.f, 25.f }, { 90.f, 0.f, 0.f }, { 10.f, 10.f, 10.f }),
    &mRenderer.mFrameBufferDeferred.mTextureDepth,
  });
}
void PlayerManager::Debug()
{
  RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
  RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
  RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });

  return;

  for (u32 i{}; i < 2; i++)
  {
    r32v3 lightPosition{ mRenderer.mUniformBlockPointLights[i].mPosition };
    r32 lightRadius{ mRenderer.mUniformBlockPointLights[i].mRadius };

    RendererLineBatchPushLine(mRenderer, lightPosition, { lightPosition.x, 0.f, lightPosition.z }, { 1.f, 1.f, 1.f, 1.f });
    RendererLineBatchPushBox(mRenderer, lightPosition, { lightRadius * 2.f, lightRadius * 2.f, lightRadius * 2.f }, { 1.f, 1.f, 1.f, 1.f });
  }

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