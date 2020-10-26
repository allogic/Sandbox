#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/RayTracing.h>

void SceneRayTracing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 1000.f);
}
void SceneRayTracing::OnDisable()
{

}
void SceneRayTracing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);
}
void SceneRayTracing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);
}
void SceneRayTracing::OnRender() const
{

}
void SceneRayTracing::OnGizmos(r32 timeDelta)
{
  u32 size{ 32 };

  for (u32 i{}; i <= size; i++)
    for (u32 j{}; j <= size; j++)
    {
      r32v3 spacing{ 1.f, 0.f, 1.f };
      r32v3 half{ size / 2, 0.f, size / 2 };
      r32v3 startP0{ i, 0, 0 };
      r32v3 startP1{ 0, 0, j };
      r32v3 endP0{ i, 0, size };
      r32v3 endP1{ size, 0, j };
      r32v4 color{ (i / (r32)size), (j / (r32)size), 0.f, 1.f };

      startP0 *= spacing;
      startP1 *= spacing;
      startP0 += -half * spacing;
      startP1 += -half * spacing;

      endP0 *= spacing;
      endP1 *= spacing;
      endP0 += -half * spacing;
      endP1 += -half * spacing;

      GizmoLineBatchPushLine(startP0, endP0, color);
      GizmoLineBatchPushLine(startP1, endP1, color);
    }
}