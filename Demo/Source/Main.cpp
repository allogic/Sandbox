#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

std::string const sVertexShaderSource
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec3 lNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 mTransform;

out vec3 position;
out vec3 normal;

void main()
{
  position = vec3(mTransform * vec4(lPosition, 1.0f));
  normal = vec3(mTransform * vec4(lNormal, 1.0f));

  gl_Position = uProjection * uView * vec4(position, 1.0f);
}
)glsl"
};

std::string const sFragmentShaderSource
{
R"glsl(
#version 460 core

in vec3 position;
in vec3 normal;

out vec4 color;

void main()
{
  color = vec4(normal, 1.0f);
}
)glsl"
};

s32    sStatus{};
u32    sWidth { 1280 };
u32    sHeight{ 720 };
r32    sAspect{ (r32)sWidth / sHeight };

Model  sModel {};
Shader sShader{};
Mesh   sMesh  {};
Camera sCamera{};

struct : Context {} sContext{};

r32 sTime{};
r32 sTimeDelta{};
r32 sTimePrev{};
r32 sTimePrevRender{};

s32 main()
{
  r32 const renderRate{ 1.f / 60 };

  r32m4 projection{};
  r32m4 view{};

  sOnWindowClose = [](GLFWwindow* pHdl)
  {
    sStatus = 1;
  };
  sOnMouseMove = [](GLFWwindow* pHdl, r64 x, r64 y)
  {
    r32v2 mousePosition{ (r32)x, (r32)y };
    r32v2 mousePositionDelta{ r32v2{ (r32)sWidth / 2, (r32)sHeight / 2 } - mousePosition };
    sCamera.mRotationDrag = sCamera.mLockDrag ? r32v2{} : r32v2{ mousePositionDelta.x, -mousePositionDelta.y };
  };
  sOnMouseButton = [](GLFWwindow* pHdl, s32 button, s32 action, s32 mods)
  {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) sCamera.mLockDrag = 0;
    else                                                                                      sCamera.mLockDrag = 1;
  };
  sOnKeyboard = [](GLFWwindow* pHdl, s32 key, s32 scancode, s32 action, s32 mods)
  {
    if (key == GLFW_KEY_A) sCamera.mPosition += sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;
    if (key == GLFW_KEY_D) sCamera.mPosition += -sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;

    if (key == GLFW_KEY_S) sCamera.mPosition += -sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;
    if (key == GLFW_KEY_W) sCamera.mPosition += sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;
  };

  CreateContext(sContext, sWidth, sHeight, "Sandbox");
  CreateModel(sModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\mod_room.fbx");
  CreateShader(sShader, sVertexShaderSource, sFragmentShaderSource);
  CreateCamera(sCamera, { 0.f, 0.f, -50.f }, 45.f, sAspect, 0.001f, 1000.f);

  while (!sStatus)
  {
    glfwPollEvents();

    sTime = (r32)glfwGetTime();
    sTimeDelta = sTime - sTimePrev;

    if ((sTime - sTimePrevRender) >= renderRate)
    {
      glClearColor(0.1f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      UpdateCamera(sCamera, sAspect, sTimeDelta);

      BindShader(sShader);
      ApplyUniformMat4(sShader, "uProjection", sCamera.mProjection);
      ApplyUniformMat4(sShader, "uView", sCamera.mView);
      ApplyUniformMat4(sShader, "mTransform", sModel.mTransform);
      BindModel(sModel);

      glfwSwapBuffers(sContext.mpWindow);

      sTimePrevRender = sTime;
    }

    sTimePrev = sTime;
  }

  glfwDestroyWindow(sContext.mpWindow);

  return 0;
}