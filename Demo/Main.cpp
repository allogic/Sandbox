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

  while (!WindowStatus())
  {
    EventsPoll();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    if (KeyDown(GLFW_KEY_1)) SceneSwitch(0);
    if (KeyDown(GLFW_KEY_2)) SceneSwitch(1);
    if (KeyDown(GLFW_KEY_3)) SceneSwitch(2);

    SceneActive()->OnUpdate(sTimeDelta);

    if ((sTime - sTimeRenderPrev) >= sTimeRender)
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

      sTimeRenderPrev = sTime;
    }

    sTimePrev = sTime;
  }

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}