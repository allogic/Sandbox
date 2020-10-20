#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/DebugUtilities.h>
#include <Scenes/ModelLoading.h>
#include <Scenes/RayTracing.h>

GLFWwindow* pWindow{};

r32 sTime          {};
r32 sTimeDelta     {};
r32 sTimePrev      {};
r32 sTimePrevRender{};

s32 main()
{
  r32 const fps{ 1.f / 60 };

  sOnWindowClose = [](GLFWwindow* pHdl)
  {
    sStatus = 1;
  };
  sOnMouseMove = [](GLFWwindow* pHdl, r64 x, r64 y)
  {
    //r32v2 mousePosition{ (r32)x, (r32)y };
    //r32v2 mousePositionDelta{ r32v2{ (r32)sWidth / 2, (r32)sHeight / 2 } - mousePosition };
    //sCamera.mRotationDrag = sCamera.mLockDrag ? r32v2{} : r32v2{ mousePositionDelta.x, -mousePositionDelta.y };
  };
  sOnMouseButton = [](GLFWwindow* pHdl, s32 button, s32 action, s32 mods)
  {
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) sCamera.mLockDrag = 0;
    //else                                                                                      sCamera.mLockDrag = 1;
  };
  sOnKeyboard = [](GLFWwindow* pHdl, s32 key, s32 scancode, s32 action, s32 mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_1: SceneSwitch(0); break;
        case GLFW_KEY_2: SceneSwitch(1); break;
        case GLFW_KEY_3: SceneSwitch(2); break;
      }
    }

    //if (key == GLFW_KEY_A) sCamera.mPosition += sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;
    //if (key == GLFW_KEY_D) sCamera.mPosition += -sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;
    //
    //if (key == GLFW_KEY_S) sCamera.mPosition += -sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;
    //if (key == GLFW_KEY_W) sCamera.mPosition += sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;
  };

  ContextCreate(pWindow, sWidth, sHeight, "Sandbox");

  SceneCreate(new SceneDebugUtilities);
  SceneCreate(new SceneModelLoading);
  SceneCreate(new SceneRayTracing);

  Scene* pScene{};

  LineBatchCreate();

  while (!sStatus)
  {
    glfwPollEvents();

    SceneActive(pScene);

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    pScene->OnUpdate(sTimeDelta);

    if ((sTime - sTimePrevRender) >= fps)
    {
      LineBatchBegin();

      pScene->OnRender();

      LineBatchEnd();
      LineBatchRender();

      glfwSwapBuffers(pWindow);

      sTimePrevRender = sTime;
    }

    sTimePrev = sTime;
  }

  LineBatchDestroy();

  SceneDestroyAll();

  glfwDestroyWindow(pWindow);

  return 0;
}