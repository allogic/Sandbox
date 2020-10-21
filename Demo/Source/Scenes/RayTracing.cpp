#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/RayTracing.h>

void SceneRayTracing::OnEnable()
{

}

void SceneRayTracing::OnDisable()
{

}

void SceneRayTracing::OnUpdate(r32 timeDelta)
{

}

void SceneRayTracing::OnRender() const
{
  glClearColor(0.f, 0.f, 0.2f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
}