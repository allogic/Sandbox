#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>

void SceneDebugUtilities::OnEnable(r32 aspect)
{

}

void SceneDebugUtilities::OnDisable()
{

}

void SceneDebugUtilities::OnUpdate(r32 timeDelta)
{

}

void SceneDebugUtilities::OnRender() const
{
  glClearColor(0.2f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
}