#include <BackgroundManager.h>

BackgroundManager::BackgroundManager(u32 numStars)
  : mNumStars{ numStars }
{
  MeshFrom(mMeshStar, SANDBOX_ENGINE_ROOT_PATH "Model\\Quad.obj");
  TextureFrom(mTextureStar, SANDBOX_ENGINE_ROOT_PATH "Texture\\Star.png");

  BufferLayoutCreate(mBufferStarTransform, mNumStars);

  ComputeMaterialCreate(mMaterialComputeStarField, "StarField");

  InitializeStarTransforms();
}
BackgroundManager::~BackgroundManager()
{
  MeshLayoutDestroy(mMeshStar);
  TextureLayoutDestroy(mTextureStar);

  BufferLayoutDestroy(mBufferStarTransform);

  ComputeMaterialDestroy(mMaterialComputeStarField);
}

void BackgroundManager::Update(r32 timeDelta)
{

}
void BackgroundManager::UpdatePhysics(r32 timeDelta)
{
  BufferLayoutBind(mBufferStarTransform);
  BufferLayoutMap(mBufferStarTransform, 0);

  //ComputeMaterialCompute(mMaterialComputeStarField);
}
void BackgroundManager::Render()
{
  RendererSubmitLambertInstanced(mRenderer, TaskLambertInstanced
  {
    &mMeshStar,
    TransformTo({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }),
    &mBufferStarTransform,
    &mTextureStar,
    nullptr,
    nullptr,
    mNumStars,
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