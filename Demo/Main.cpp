#include <Api.h>

#include <Scenes/Game.h>

r32 sTime     {};
r32 sTimePrev {};
r32 sTimeDelta{};

r32 sTimeRender    { 1.f / 60 };
r32 sTimeRenderPrev{};

int main(int argc, char** argv)
{
  ContextCreate(1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  SceneCreate(new SceneGame);

  GizmoLineBatchCreate();

  DeferredRenderCreate();

  while (!WindowStatus())
  {
    EventsPoll();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimeRenderPrev) >= sTimeRender)
    {
      SceneActive()->OnUpdateFixed(sTimeDelta);

      DeferredRenderPassGeometry(sTimeDelta);
      DeferredRenderPassLight(sTimeDelta);

      //GizmoLineBatchBind();
      //SceneActive()->OnGizmos(sTimeDelta);
      //GizmoLineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimeRenderPrev = sTime;
    }

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  DeferredRenderDestroy();

  ContextDestroy();

  return 0;
}