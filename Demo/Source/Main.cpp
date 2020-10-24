#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>
#include <Scenes/ModelLoading.h>
#include <Scenes/RayTracing.h>

r32 sTime     {};
r32 sTimePrev {};
r32 sTimeDelta{};

r32 sTimeRenderFixed     { 1.f / 60 };
r32 sTimeRenderFixedPrev {};
r32 sTimeRenderFixedDelta{};

s32 main()
{
  ContextCreate(1280, 720, "Sandbox");

  SceneCreate(new SceneDebugUtilities);
  SceneCreate(new SceneModelLoading);
  SceneCreate(new SceneRayTracing);

  LineBatchCreate(65536);

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
      sTimeRenderFixedDelta = sTime - sTimeRenderFixedPrev;

      SceneActive()->OnUpdateFixed(sTimeRenderFixedDelta);
      SceneActive()->OnRender();

      LineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimeRenderFixedPrev = sTime;
    }

    // bind once
    SceneActive()->OnGizmos(sTimeDelta);
    // draw once

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}