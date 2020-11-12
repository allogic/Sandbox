#include <Api.h>

GLFWwindow* spWindow{};

std::vector<Scene*> sScenes      {};
Scene*              spSceneActive{};

s32   sStatus{};
u32   sWidth {};
u32   sHeight{};
r32   sAspect{};

r32v2 sMousePosition{};
r32v2 sMouseScroll  {};

Event sMouseKeyStates[GLFW_MOUSE_BUTTON_LAST]{};
Event sKeyboardKeyStates[GLFW_KEY_LAST]      {};

u32         sGizmoLineBatchVertexBufferSize{ 65535 };
ShaderGizmo sGizmoLineBatchShader          {};
MeshGizmo   sGizmoLineBatchMesh            {};
u32         sGizmoLineBatchOffsetVertex    {};
u32         sGizmoLineBatchOffsetIndex     {};

/*
* OpenGL context specific.
*/

void        ContextCreate(u32 width, u32 height, std::string const& title)
{
  sWidth = width;
  sHeight = height;
  sAspect = (r32)width / height;

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  spWindow = glfwCreateWindow((s32)width, (s32)height, title.data(), nullptr, nullptr);

  glfwSetWindowCloseCallback(spWindow, [](GLFWwindow*)
    {
      sStatus = 1;
    });
  glfwSetWindowSizeCallback(spWindow, [](GLFWwindow*, s32 width, s32 height)
    {
      sWidth = (u32)width;
      sHeight = (u32)height;
      sAspect = (r32)width / height;

      glViewport(0, 0, width, height);
    });
  glfwSetCursorPosCallback(spWindow, [](GLFWwindow*, r64 x, r64 y)
    {
      sMousePosition = { (r32)x, (r32)y };
    });
  glfwSetScrollCallback(spWindow, [](GLFWwindow*, r64 xOffset, r64 yOffset)
    {
      sMouseScroll = { (r32)xOffset, (r32)yOffset };
    });

  glfwMakeContextCurrent(spWindow);
  glfwSwapInterval(0);

  gladLoadGL();
}
void        ContextRegisterDebugHandler()
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
GLFWwindow* ContextHandle()
{
  return spWindow;
}
void        ContextDestroy()
{
  glfwDestroyWindow(spWindow);
}

/*
* Event dispatching.
*/

void EventsPoll()
{
  sMouseScroll = {};

  glfwPollEvents();

  for (u32 i{}; i < GLFW_MOUSE_BUTTON_LAST; i++)
  {
    s32 action{ glfwGetMouseButton(spWindow, (s32)i) };
    Event& event{ sMouseKeyStates[i] };

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
    Event& event{ sKeyboardKeyStates[i] };
    s32 action{ glfwGetKey(spWindow, (s32)i) };

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
* Mouse/Keyboard/Window state handling.
*/

u32 WindowStatus()
{
  return sStatus;
}
r32 WindowSizeX()
{
  return (r32)sWidth;
}
r32 WindowSizeY()
{
  return (r32)sHeight;
}
r32 WindowAspect()
{
  return sAspect;
}
r32 MousePositionX()
{
  return sMousePosition.x;
}
r32 MousePositionY()
{
  return sMousePosition.y;
}
r32 MouseScrollX()
{
  return sMouseScroll.x;
}
r32 MouseScrollY()
{
  return sMouseScroll.y;
}
u32 MouseDown(u32 key)
{
  return sMouseKeyStates[key].mActionCurr == eEventDown;
}
u32 MouseHeld(u32 key)
{
  return sMouseKeyStates[key].mActionCurr == eEventHeld;
}
u32 MouseUp(u32 key)
{
  return sMouseKeyStates[key].mActionCurr == eEventUp;
}
u32 KeyDown(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == eEventDown;
}
u32 KeyHeld(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == eEventHeld;
}
u32 KeyUp(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == eEventUp;
}

/*
* Scene management.
*/

void   SceneCreate(Scene* pScene)
{
  sScenes.emplace_back(pScene);

  if (!spSceneActive)
  {
    spSceneActive = sScenes.back();
    spSceneActive->OnEnable();
  }
}
void   SceneSwitch(u32 index)
{
  GizmoLineBatchClear();

  spSceneActive->OnDisable();
  spSceneActive = sScenes[index];
  spSceneActive->OnEnable();
}
Scene* SceneActive()
{
  return spSceneActive;
}
void   SceneDestroyAll()
{
  spSceneActive = nullptr;

  for (auto pScene : sScenes)
  {
    pScene->OnDisable();
    delete pScene;
  }

  sScenes.clear();
}

/*
* Model management.
*/

void ModelCreate(ModelLambert& model, std::string const& fileName)
{
  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType) };

  u32 const numMeshes{ pScene->mNumMeshes };
  u32 const numMaterials{ pScene->mNumMaterials };
  u32 const numTextures{ pScene->mNumTextures };

  std::vector<u32> vertexBufferSizes{};
  std::vector<u32> indexBufferSizes{};
  std::vector<std::vector<VertexLambert>> vertexBuffers{};
  std::vector<std::vector<u32>> indexBuffers{};

  vertexBufferSizes.resize(numMeshes);
  indexBufferSizes.resize(numMeshes);
  vertexBuffers.resize(numMeshes);
  indexBuffers.resize(numMeshes);

  for (u32 i{}; i < numMeshes; i++)
  {
    aiMesh const* pMesh{ pScene->mMeshes[i] };

    if (!(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
      continue;

    vertexBufferSizes[i] = pMesh->mNumVertices;
    indexBufferSizes[i] = pMesh->mNumFaces * 3;

    vertexBuffers[i].resize(pMesh->mNumVertices);
    indexBuffers[i].resize(pMesh->mNumFaces * 3);

    for (u32 j{}; j < pMesh->mNumVertices; j++)
    {
      vertexBuffers[i][j] =
      {
        .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
        .mNormal  { pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
        .mUv      { pMesh->mTextureCoords[0][j].x, pMesh->mTextureCoords[0][j].y },
        .mColor   { 0.f, 0.f, 0.f, 1.f },
      };
    }
    for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
    {
      indexBuffers[i][k + 0] = { pMesh->mFaces[j].mIndices[0] };
      indexBuffers[i][k + 1] = { pMesh->mFaces[j].mIndices[1] };
      indexBuffers[i][k + 2] = { pMesh->mFaces[j].mIndices[2] };
    }
  }
  for (u32 i{}; i < numMaterials; i++)
  {
    aiMaterial const* pMaterial{ pScene->mMaterials[i] };

    for (u32 j{}; j < pMaterial->mNumProperties; j++)
    {
      //std::printf("%s\n", pMaterial->mProperties[j]->mKey.data);
    }
  }
  for (u32 i{}; i < numTextures; i++)
  {
    aiTexture const* pTexture{ pScene->mTextures[i] };
  }

  ModelLayoutCreate(model, numMeshes, vertexBufferSizes.data(), indexBufferSizes.data());
  
  for (u32 i{}; i < numMeshes; i++)
  {
    ModelLayoutBind(model, i);
    ModelLayoutData(model, i, vertexBuffers[i].data(), indexBuffers[i].data());
  }
}
void ModelRender(ModelLambert const& model)
{
  for (u32 i{}; i < model.mNumSubMeshes; i++)
  {
    ModelLayoutBind(model, i);
    ModelLayoutRender(model, i, eRenderModeTriangle);
  }
}
void ModelRenderInstanced(ModelLambert const& model, u32 numInstances)
{
  for (u32 i{}; i < model.mNumSubMeshes; i++)
  {
    ModelLayoutBind(model, i);
    ModelLayoutRenderInstanced(model, i, eRenderModeTriangle, numInstances);
  }
}
void ModelDestroy(ModelLambert const& model)
{
  ModelLayoutDestroy(model);
}

/*
* 3D debug utilities.
*/

void GizmoLineBatchCreate()
{
  ShaderLayoutCreate(sGizmoLineBatchShader, ShaderPaths{ .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.vert" }, .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.frag" } });
  MeshLayoutCreate(sGizmoLineBatchMesh, sGizmoLineBatchVertexBufferSize, sGizmoLineBatchVertexBufferSize * 2);
}
void GizmoLineBatchClear()
{
  MeshLayoutBind(sGizmoLineBatchMesh);
  MeshLayoutClear(sGizmoLineBatchMesh);

  sGizmoLineBatchOffsetVertex = 0;
  sGizmoLineBatchOffsetIndex = 0;
}
void GizmoLineBatchBind()
{
  MeshLayoutBind(sGizmoLineBatchMesh);
}
void GizmoLineBatchPushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  VertexGizmo vertices[2]{ { p0, color }, { p1, color } };
  u32 indices[2]{ sGizmoLineBatchOffsetVertex + 0, sGizmoLineBatchOffsetVertex + 1 };

  MeshLayoutDataSub(sGizmoLineBatchMesh, vertices, indices, sGizmoLineBatchOffsetVertex, sGizmoLineBatchOffsetIndex, 2, 2);
  
  sGizmoLineBatchOffsetVertex += 2;
  sGizmoLineBatchOffsetIndex += 2;
}
void GizmoLineBatchPushBox(r32v3 const& position, r32v3 const& size, r32v4 const& color)
{
  r32v3 half{ size / 2.f };

  r32v3 blf{ -half.x, -half.y, -half.z };
  r32v3 brf{  half.x, -half.y, -half.z };
  r32v3 tlf{ -half.x,  half.y, -half.z };
  r32v3 trf{  half.x,  half.y, -half.z };

  r32v3 blb{ -half.x, -half.y,  half.z };
  r32v3 brb{  half.x, -half.y,  half.z };
  r32v3 tlb{ -half.x,  half.y,  half.z };
  r32v3 trb{  half.x,  half.y,  half.z };

  VertexGizmo vertices[8]
  {
    // front
    { position + blf, color },
    { position + brf, color },
    { position + tlf, color },
    { position + trf, color },

    // back
    { position + blb, color },
    { position + brb, color },
    { position + tlb, color },
    { position + trb, color },
  };
  u32 indices[24]
  {
    // front
    sGizmoLineBatchOffsetVertex + 0,
    sGizmoLineBatchOffsetVertex + 1,
    sGizmoLineBatchOffsetVertex + 0,
    sGizmoLineBatchOffsetVertex + 2,
    sGizmoLineBatchOffsetVertex + 2,
    sGizmoLineBatchOffsetVertex + 3,
    sGizmoLineBatchOffsetVertex + 3,
    sGizmoLineBatchOffsetVertex + 1,

    // back
    sGizmoLineBatchOffsetVertex + 4,
    sGizmoLineBatchOffsetVertex + 5,
    sGizmoLineBatchOffsetVertex + 4,
    sGizmoLineBatchOffsetVertex + 6,
    sGizmoLineBatchOffsetVertex + 6,
    sGizmoLineBatchOffsetVertex + 7,
    sGizmoLineBatchOffsetVertex + 7,
    sGizmoLineBatchOffsetVertex + 5,

    // connections
    sGizmoLineBatchOffsetVertex + 0,
    sGizmoLineBatchOffsetVertex + 4,
    sGizmoLineBatchOffsetVertex + 1,
    sGizmoLineBatchOffsetVertex + 5,
    sGizmoLineBatchOffsetVertex + 2,
    sGizmoLineBatchOffsetVertex + 6,
    sGizmoLineBatchOffsetVertex + 3,
    sGizmoLineBatchOffsetVertex + 7,
  };

  MeshLayoutDataSub(sGizmoLineBatchMesh, vertices, indices, sGizmoLineBatchOffsetVertex, sGizmoLineBatchOffsetIndex, 8, 24);

  sGizmoLineBatchOffsetVertex += 8;
  sGizmoLineBatchOffsetIndex += 24;
}
void GizmoLineBatchRender()
{
  ShaderLayoutBind(sGizmoLineBatchShader);
  MeshLayoutRender(sGizmoLineBatchMesh, eRenderModeLine);

  sGizmoLineBatchOffsetVertex = 0;
  sGizmoLineBatchOffsetIndex = 0;
}