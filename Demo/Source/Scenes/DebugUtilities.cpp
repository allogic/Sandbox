#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>

void SceneDebugUtilities::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 10.f }, 45.f, 0.001f, 1000.f);
}

void SceneDebugUtilities::OnDisable()
{

}

void SceneDebugUtilities::OnUpdate(r32 timeDelta)
{
  CameraUpdateController(mCamera, mCameraController, timeDelta);
}

void SceneDebugUtilities::OnRender() const
{
  glClearColor(0.2f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  LineBatchPush({ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f });
  LineBatchPush({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f });
  LineBatchPush({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }, { 0.f, 0.f, 1.f, 1.f });
}