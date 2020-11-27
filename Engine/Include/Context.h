#pragma once

#include <Core.h>
#include <Types.h>
#include <Events.h>
#include <Registry.h>
#include <Renderer.h>
#include <Scene.h>

/*
* Global context.
*/

struct Context
{
  GLFWwindow*         mpWindow                               {};
  u32                 mStatus                                {};
  u32                 mWidth                                 {};
  u32                 mHeight                                {};
  r32v2               mMousePosition                         {};
  r32v2               mMouseScroll                           {};
  Event               mMouseKeyStates[GLFW_MOUSE_BUTTON_LAST]{};
  Event               mKeyboardKeyStates[GLFW_KEY_LAST]      {};
  std::vector<Scene*> mScenes                                {};
  Scene*              mpSceneActive                          {};
};

/*
* Context event management.
*/

template<typename Context> void ContextEventsPoll(Context& context)
{
  context.mMouseScroll = {};

  glfwPollEvents();

  for (u32 i{}; i < GLFW_MOUSE_BUTTON_LAST; i++)
  {
    s32 action{ glfwGetMouseButton(context.mpWindow, (s32)i) };
    Event& event{ context.mMouseKeyStates[i] };

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

  for (u32 i{}; i < GLFW_KEY_LAST; i++)
  {
    Event& event{ context.mKeyboardKeyStates[i] };
    s32 action{ glfwGetKey(context.mpWindow, (s32)i) };

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

/*
* Context management.
*/

template<typename Context> void ContextCreate(Context& context, u32 width, u32 height, std::string const& title)
{
  context.mWidth = width;
  context.mHeight = height;

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  context.mpWindow = glfwCreateWindow((s32)context.mWidth, (s32)context.mHeight, title.data(), nullptr, nullptr);

  glfwSetWindowUserPointer(context.mpWindow, &context);

  glfwSetWindowCloseCallback(context.mpWindow, [](GLFWwindow* pWindow)
    {
      Context* pContext = (Context*)glfwGetWindowUserPointer(pWindow);

      pContext->mStatus = 1;
    });
  glfwSetWindowSizeCallback(context.mpWindow, [](GLFWwindow* pWindow, s32 width, s32 height)
    {
      Context* pContext = (Context*)glfwGetWindowUserPointer(pWindow);

      pContext->mWidth = (u32)width;
      pContext->mHeight = (u32)height;

      glViewport(0, 0, width, height);
    });
  glfwSetCursorPosCallback(context.mpWindow, [](GLFWwindow* pWindow, r64 x, r64 y)
    {
      Context* pContext = (Context*)glfwGetWindowUserPointer(pWindow);

      pContext->mMousePosition = { (r32)x, (r32)y };
    });
  glfwSetScrollCallback(context.mpWindow, [](GLFWwindow* pWindow, r64 xOffset, r64 yOffset)
    {
      Context* pContext = (Context*)glfwGetWindowUserPointer(pWindow);

      pContext->mMouseScroll = { (r32)xOffset, (r32)yOffset };
    });

  glfwMakeContextCurrent(context.mpWindow);
  glfwSwapInterval(0);

  gladLoadGL();
}
static                     void ContextRegisterDebugHandler()
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
template<typename Context> void ContextRun(Context& context)
{
  r32& time{ RegistryGetOrCreate<r32>("time") };
  r32 timePrev{};
  r32& timeDelta{ RegistryGetOrCreate<r32>("timeDelta") };

  r32 timeRender{ 1.f / 60 };
  r32 timeRenderPrev{};

  Renderer& renderer{ RegistryGetOrCreate<Renderer>("renderer") };

  RendererCreate(renderer, context.mWidth, context.mHeight);

  while (!context.mStatus)
  {
    ContextEventsPoll(context);

    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;

    context.mpSceneActive->OnUpdate(timeDelta);

    if ((time - timeRenderPrev) >= timeRender)
    {
      context.mpSceneActive->OnUpdateFixed(timeDelta);
      context.mpSceneActive->OnRender();

      MeshLayoutBind(renderer.mMeshGizmoLineBatch, 0);
      context.mpSceneActive->OnGizmos(timeDelta);
      MeshLayoutUnbind();

      RendererRenderBegin(renderer);
      context.mpSceneActive->OnRenderPre();
      RendererRender(renderer);
      context.mpSceneActive->OnRenderPost();
      RendererRenderEnd(renderer);

      glfwSwapBuffers(context.mpWindow);

      timeRenderPrev = time;
    }

    timePrev = time;
  }

  RendererDestroy(renderer);
}
template<typename Context> void ContextDestroy(Context const& context)
{
  glfwDestroyWindow(context.mpWindow);
}

/*
* Context scene management.
*/

template<typename Context> void ContextSceneCreate(Context& context, Scene* pScene)
{
  context.mScenes.emplace_back(pScene);

  if (!context.mpSceneActive)
  {
    context.mpSceneActive = context.mScenes.back();
    context.mpSceneActive->OnEnable();
  }
}
template<typename Context> void ContextSceneSwitch(Context& context, u32 index)
{
  context.mpSceneActive->OnDisable();
  context.mpSceneActive = context.mScenes[index];
  context.mpSceneActive->OnEnable();
}
template<typename Context> void ContextSceneDestroyAll(Context& context)
{
  context.mpSceneActive = nullptr;

  for (auto pScene : context.mScenes)
  {
    pScene->OnDisable();
    delete pScene;
  }

  context.mScenes.clear();
}