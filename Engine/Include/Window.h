#pragma once

#include <Core.h>
#include <Types.h>
#include <Events.h>
#include <ACS.h>
#include <Registry.h>
#include <Renderer.h>

#include <Components/RenderComponents.h>

/*
* Global context.
*/

using Context = GLFWwindow;

struct Window
{
  Context* mpContext                              {};
  u32      mStatus                                {};
  u32      mWidth                                 {};
  u32      mHeight                                {};
  r32v2    mMousePosition                         {};
  r32v2    mMouseScroll                           {};
  Event    mMouseKeyStates[GLFW_MOUSE_BUTTON_LAST]{};
  Event    mKeyboardKeyStates[GLFW_KEY_LAST - 32] {};
};

/*
* Context management.
*/

static void ContextCreateDefault(Context*& pContext, u32 width, u32 height, std::string const& title)
{
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_DECORATED, GL_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  pContext = glfwCreateWindow((s32)width, (s32)height, title.c_str(), nullptr, nullptr);

  assert(pContext);
}
static void ContextCreateChild(Context* pContextParent, Context*& pContextChild)
{
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  glfwWindowHint(GLFW_DECORATED, GL_FALSE);
  glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 0);

  pContextChild = glfwCreateWindow(1, 1, "", nullptr, pContextParent);

  assert(pContextChild);
}
static void ContextDestroy(Context* pContext)
{
  glfwDestroyWindow(pContext);
}

/*
* Window management.
*/

static                    void WindowRegisterDebugHandler()
{
  glEnable(GL_DEBUG_OUTPUT);

  glDebugMessageCallback([](u32 source, u32 type, u32 id, u32 severity, s32 length, s8 const* msg, void const* userParam)
  {
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION: break;
    case GL_DEBUG_SEVERITY_LOW: std::printf("Severity:Low Type:0x%x Message:%s\n", type, msg); break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::printf("Severity:Medium Type:0x%x Message:%s\n", type, msg); break;
    case GL_DEBUG_SEVERITY_HIGH: std::printf("Severity:High Type:0x%x Message:%s\n", type, msg); break;
    }
  }, 0);
}
template<typename Window> void WindowCreate(Window& window, u32 width, u32 height, std::string const& title)
{
  window.mWidth = width;
  window.mHeight = height;

  glfwInit();

  ContextCreateDefault(window.mpContext, width, height, title);

  glfwMakeContextCurrent(window.mpContext);

  gladLoadGL();

  WindowRegisterDebugHandler();

  glfwSwapInterval(0);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGuiIO& io{ ImGui::GetIO() };
  ImGuiStyle& style{ ImGui::GetStyle() };
  style.WindowRounding = 0.f;
  style.WindowBorderSize = 0.f;

  ImGui_ImplGlfw_InitForOpenGL(window.mpContext, 1);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  glfwSetWindowUserPointer(window.mpContext, &window);

  glfwSetErrorCallback([](s32 errorCode, s8 const* pMsg)
  {
    std::printf("Glfw Error[%u] %s\n", errorCode, pMsg);
  });

  glfwSetWindowCloseCallback(window.mpContext, [](Context* pContext)
  {
    Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

    pWindow->mStatus = 1;
  });
  glfwSetWindowSizeCallback(window.mpContext, [](Context* pContext, s32 width, s32 height)
  {
    Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

    pWindow->mWidth = (u32)width;
    pWindow->mHeight = (u32)height;

    glViewport(0, 0, width, height);
  });
  glfwSetCursorPosCallback(window.mpContext, [](Context* pContext, r64 x, r64 y)
  {
    Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

    pWindow->mMousePosition = { (r32)x, (r32)y };
  });
  glfwSetScrollCallback(window.mpContext, [](Context* pContext, r64 xOffset, r64 yOffset)
  {
    Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

    pWindow->mMouseScroll = { (r32)xOffset, (r32)yOffset };
  });

  Renderer& renderer{ RegistryGetOrCreate<Renderer>("renderer") };

  RendererCreate(renderer, window.mWidth, window.mHeight);
}
template<typename Window> void WindowEventsPoll(Window& window)
{
  window.mMouseScroll = {};

  glfwPollEvents();

  for (u32 i{}; i < GLFW_MOUSE_BUTTON_LAST; i++)
  {
    Event& event{ window.mMouseKeyStates[i] };
    s32 action{ glfwGetMouseButton(window.mpContext, (s32)i) };

    event.mActionPrev = event.mActionCurr;

    if (action == GLFW_PRESS)
    {
      if (event.mActionCurr != eEventDown && event.mActionPrev != eEventHeld)
      {
        event.mActionCurr = eEventDown;
      }
      else
      {
        event.mActionCurr = eEventHeld;
      }
    }
    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != eEventUp && event.mActionPrev == eEventHeld)
      {
        event.mActionCurr = eEventUp;
      }
      else
      {
        event.mActionCurr = eEventNone;
      }
    }
  }
  for (u32 i{ 32 }; i < GLFW_KEY_LAST; i++)
  {
    Event& event{ window.mKeyboardKeyStates[i] };
    s32 action{ glfwGetKey(window.mpContext, (s32)i) };

    if (action == GLFW_PRESS)
    {
      if (event.mActionCurr != eEventDown && event.mActionPrev != eEventHeld)
      {
        event.mActionCurr = eEventDown;
      }
      else
      {
        event.mActionCurr = eEventHeld;
      }
    }
    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != eEventUp && event.mActionPrev == eEventHeld)
      {
        event.mActionCurr = eEventUp;
      }
      else
      {
        event.mActionCurr = eEventNone;
      }
    }
  }
}
template<typename Window> void WindowRun(Window& window)
{
  r32 time{};
  r32 timePrev{};
  r32 timeDelta{};

  r32 timeRender{ 1.f / 60 };
  r32 timeRenderPrev{};

  Renderer& renderer{ RegistryGetOrCreate<Renderer>("renderer") };

  while (!window.mStatus)
  {
    WindowEventsPoll(window);

    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;

    ADB::Update();
    ACS::Update();

    ACS::Dispatch([=](Actor* pActor)
    {
      pActor->OnUpdate(time, timeDelta);
    });

    if ((time - timeRenderPrev) >= timeRender)
    {
      ACS::Dispatch([=](Actor* pActor)
      {
        pActor->OnUpdateFixed(time, timeDelta);
      });

      RendererRenderBegin(renderer, time, timeDelta);
      RendererRender(renderer);
      RendererRenderEnd(renderer);

      glfwSwapBuffers(window.mpContext);

      timeRenderPrev = time;
    }

    timePrev = time;
  }

  RendererDestroy(renderer);
}
template<typename Window> void WindowDestroy(Window const& window)
{
  ImGui::DestroyContext();

  ContextDestroy(window.mpContext);
}