#include <Scene.h>

Scene::Scene(u32 width, u32 height)
{
  GraphicBufferCreate(mGraphicBuffer, width, height);

  UniformLayoutCreate(mUniformProjection);
  UniformLayoutBind(mUniformProjection);
  //UniformLayoutMap(mUniformProjection, 0);
}
Scene::~Scene()
{
  GraphicBufferDestroy(mGraphicBuffer);

  UniformLayoutDestroy(mUniformProjection);
}