#include <BackgroundManager.h>

BackgroundManager::BackgroundManager(u32 numStars)
  : mNumStars{ numStars }
{
  ModelCreate(mModelStar, SANDBOX_ROOT_PATH "Model\\Quad.obj");
  ModelLayoutTransform(mModelStar, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });

  TextureLayoutCreate(mTextureStar, 256, 256);
  TextureLayoutBind(mTextureStar);
  TextureLayoutDataSetFrom(mTextureStar, SANDBOX_ROOT_PATH "Texture\\Star.png");

  BufferLayoutCreate(mBufferStarTransform, mNumStars);

  ComputeMaterialCreate(mMaterialComputeStarField, "StarField");

  InitializeStarTransforms();
}
BackgroundManager::~BackgroundManager()
{
  ModelDestroy(mModelStar);
  TextureLayoutDestroy(mTextureStar);

  BufferLayoutDestroy(mBufferStarTransform);

  ComputeMaterialDestroy(mMaterialComputeStarField);
}

void BackgroundManager::Update(r32 timeDelta)
{

}
void BackgroundManager::UpdatePhysics(r32 timeDelta)
{

}
void BackgroundManager::Render()
{
  RendererSubmitLambertInstanced(mRenderer, TaskLambertInstanced
  {
    &mModelStar,
    &mBufferStarTransform,
    &mTextureStar,
    mNumStars
  });
}
void BackgroundManager::Debug()
{

}

void BackgroundManager::InitializeStarTransforms()
{
  mStarTransforms.resize(mNumStars);

  for (u32 i{}; i < mNumStars; i++)
  {
    r32v3 position{ glm::ballRand(100.f) };

    mStarTransforms[i] =
    {
      .mPosition          { position },
      .mRotation          { 0.f, 0.f, 45.f },
      .mRotationLocalRight{ 1.f, 0.f, 0.f },
      .mRotationLocalUp   { 0.f, 1.f, 0.f },
      .mRotationLocalFront{ 0.f, 0.f, 1.f },
    };
  }

  BufferLayoutBind(mBufferStarTransform);
  BufferLayoutDataSet(mBufferStarTransform, mNumStars, mStarTransforms.data());
  BufferLayoutUnbind();
}