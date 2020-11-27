#include <MapManager.h>

MapManager::MapManager()
{
  MeshFrom(mMeshPlanet, SANDBOX_ROOT_PATH "Models\\Primitives\\Sphere.obj");
  TextureFrom(mTexturePlanet, SANDBOX_ROOT_PATH "Textures\\Noise.png");
}
MapManager::~MapManager()
{
  
}

void MapManager::Update(r32 timeDelta)
{

}
void MapManager::UpdatePhysics(r32 timeDelta)
{

}
void MapManager::Render()
{
  RendererSubmitLambert(mRenderer, TaskLambert
  {
    &mMeshPlanet,
    TransformTo(mSystemHome.mSun.mPosition, mSystemHome.mSun.mRotation, mSystemHome.mSun.mScale),
    &mTexturePlanet,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
  });

  for (u32 i{}; i < mSystemHome.mPlanets.size(); i++)
  {
    RendererSubmitLambert(mRenderer, TaskLambert
    {
      &mMeshPlanet,
      TransformTo(mSystemHome.mPlanets[i].mPosition, mSystemHome.mPlanets[i].mRotation, mSystemHome.mPlanets[i].mScale),
      &mTexturePlanet,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    });
  }
}
void MapManager::Debug()
{
  for (u32 i{}; i < mSystemHome.mPlanets.size(); i++)
  {
    RendererLineBatchPushBox(mRenderer, mSystemHome.mSun.mPosition, mSystemHome.mSun.mScale, { 1.f, 1.f, 1.f, 1.f });
  }
}