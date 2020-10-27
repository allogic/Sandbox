#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/ModelLoading.h>

void SceneModelLoading::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 1000.f);
  ModelCreate(mModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\teapot.fbx");
  ShaderCreateRender(mShaderRender, mShaderRenderVertexLambert, mShaderRenderFragmentLambert);
}
void SceneModelLoading::OnDisable()
{
  ModelDestroy(mModel);
  ShaderDestroyRender(mShaderRender);
}
void SceneModelLoading::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);
}
void SceneModelLoading::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);
}
void SceneModelLoading::OnRender(r32 timeDelta) const
{
  ShaderBind(mShaderRender);
  ShaderUniformR32M4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRender, "uView", mCamera.mView);
  ModelRender(mModel);
}
void SceneModelLoading::OnGizmos(r32 timeDelta)
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