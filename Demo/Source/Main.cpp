#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

s32 main()
{
  r32 time{};
  r32 timeDelta{};
  r32 timePrev{};
  r32 timeRenderPrev{};

  r32 const renderRate{ 1.f / 60 };

  GLFWwindow* pWindow{};

  Mesh mesh{};
  Material material{};

  CreateWindow(pWindow, 1280, 720, "Sandbox");
  CreateModel(mesh, material, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\cozy_room.fbx");

  while (1)
  {
    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;

    if ((time - timeRenderPrev) >= renderRate)
    {
      glClearColor(0.1f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      timeRenderPrev = time;
    }

    glfwSwapBuffers(pWindow);
    glfwPollEvents();

    timePrev = time;
  }

  glfwDestroyWindow(pWindow);

  return 0;
}