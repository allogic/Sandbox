#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>
#include <Scenes/ModelLoading.h>
#include <Scenes/RayTracing.h>

r32 sTimeRenderFixed    { 1.f / 60 };
r32 sTimeRenderFixedPrev{};

r32 sTime     {};
r32 sTimeDelta{};
r32 sTimePrev {};

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

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimeRenderFixedPrev) >= sTimeRenderFixed)
    {
      SceneActive()->OnUpdateFixed(sTimeDelta);
      SceneActive()->OnRender();

      LineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimeRenderFixedPrev = sTime;
    }

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}