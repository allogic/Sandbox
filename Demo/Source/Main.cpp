#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

extern u32 status;

std::string const vertexShaderSource
{
R"glsl(
void main()
{

}
)glsl"
};

std::string const fragmentShaderSource
{
R"glsl(
void main()
{

}
)glsl"
};

s32 main()
{
  r32 time{};
  r32 timeDelta{};
  r32 timePrev{};
  r32 timeRenderPrev{};

  r32 const renderRate{ 1.f / 60 };

  Window window{};
  Model model{};
  Shader shader{};

  CreateWindow(window, 1280, 720, "Sandbox");
  CreateModel(model, "C:\\Users\\Burmi\\Downloads\\Sandbox\\Models\\cozy_room.fbx");
  CreateShader(shader, vertexShaderSource, fragmentShaderSource);

  while (!status)
  {
    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;

    if ((time - timeRenderPrev) >= renderRate)
    {
      glClearColor(0.1f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      timeRenderPrev = time;
    }

    glfwSwapBuffers(window.mpHdl);
    glfwPollEvents();

    timePrev = time;
  }

  glfwDestroyWindow(window.mpHdl);

  return 0;
}