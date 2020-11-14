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

  SceneCreate(new SceneGame(1280, 720));

  GizmoLineBatchCreate();

  while (!WindowStatus())
  {
    EventsPoll();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimeRenderPrev) >= sTimeRender)
    {
      SceneActive()->OnUpdateFixed(sTimeDelta);

      SANDBOX_ENGINE_MEASURE_BEGIN(GraphicBufferDeferredPassGeometry);
      GraphicBufferDeferredPassGeometryBegin(SceneActive()->mGraphicBuffer);
      SceneActive()->OnRender(sTimeDelta);
      GraphicBufferDeferredPassGeometryEnd(SceneActive()->mGraphicBuffer);
      SANDBOX_ENGINE_MEASURE_END(GraphicBufferDeferredPassGeometry);

      SANDBOX_ENGINE_MEASURE_BEGIN(GraphicBufferDeferredPassLight);
      GraphicBufferDeferredPassLightBegin(SceneActive()->mGraphicBuffer);
      GraphicBufferDeferredPassLightEnd(SceneActive()->mGraphicBuffer);
      SANDBOX_ENGINE_MEASURE_END(GraphicBufferDeferredPassLight);

      //GizmoLineBatchBind();
      //SceneActive()->OnGizmos(sTimeDelta);
      //GizmoLineBatchRender();

      glfwSwapBuffers(ContextHandle());

      sTimeRenderPrev = sTime;
    }

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}