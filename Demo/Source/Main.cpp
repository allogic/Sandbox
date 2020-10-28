#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>
#include <Scenes/ModelLoading.h>
#include <Scenes/RayTracing.h>

r32 sTime     {};
r32 sTimePrev {};
r32 sTimeDelta{};

r32 sTimeRenderFixed     { 1.f / 60 };
r32 sTimeRenderFixedPrev {};

s32 main()
{
  ContextCreate(1280, 720, "Sandbox");

  SceneCreate(new SceneInstancing);
  SceneCreate(new SceneModelLoading);
  SceneCreate(new SceneRayTracing);

  GizmoLineBatchCreate();

  while (!WindowStatus())
  {
    EventsPoll();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    if (KeyDown(GLFW_KEY_1)) SceneSwitch(0);
    if (KeyDown(GLFW_KEY_2)) SceneSwitch(1);
    if (KeyDown(GLFW_KEY_3)) SceneSwitch(2);

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimeRenderFixedPrev) >= sTimeRenderFixed)
    {
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glClearColor(0.f, 0.f, 0.f, 0.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      SceneActive()->OnUpdateFixed(sTimeDelta);
      SceneActive()->OnRender(sTimeDelta);

      GizmoLineBatchBind();
      SceneActive()->OnGizmos(sTimeDelta);
      GizmoLineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimeRenderFixedPrev = sTime;
    }

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}