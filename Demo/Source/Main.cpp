#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>
#include <Scenes/ModelLoading.h>
#include <Scenes/RayTracing.h>

r32 sTime          {};
r32 sTimeDelta     {};
r32 sTimePrev      {};
r32 sTimePrevRender{};

s32 main()
{
  r32 const fps{ 1.f / 60 };

  ContextCreate(1280, 720, "Sandbox");

  SceneCreate(new SceneDebugUtilities);
  SceneCreate(new SceneModelLoading);
  SceneCreate(new SceneRayTracing);

  LineBatchCreate();

  while (!WindowStatus())
  {
    glfwPollEvents();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimePrevRender) >= fps)
    {
      LineBatchBegin();

      SceneActive()->OnRender();

      LineBatchEnd();
      LineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimePrevRender = sTime;
    }

    EventStateNext();

    sTimePrev = sTime;
  }

  LineBatchDestroy();

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}