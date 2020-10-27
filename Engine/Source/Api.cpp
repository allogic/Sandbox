#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

GLFWwindow* spWindow{};

std::vector<Scene*> sScenes      {};
Scene*              spSceneActive{};

s32   sStatus       {};
u32   sWidth        {};
u32   sHeight       {};
r32   sAspect       {};

r32v2 sMousePosition{};
r32v2 sMouseScroll  {};

Event sMouseKeyStates[GLFW_MOUSE_BUTTON_LAST]{};
Event sKeyboardKeyStates[GLFW_KEY_LAST]      {};

std::string const sRenderShaderVertexGizmo
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec4 lColor;

uniform mat4 uProjection;
uniform mat4 uView;

out vec3 fPosition;
out vec4 fColor;

void main()
{
  fPosition = lPosition;
  fColor = lColor;

  gl_Position = uProjection * uView * vec4(fPosition, 1.0f);
}
)glsl"
};
std::string const sRenderShaderFragmentGizmo
{
R"glsl(
#version 460 core

in vec3 fPosition;
in vec4 fColor;

out vec4 color;

void main()
{
  color = fColor;
}
)glsl"
};

u32          sGizmoLineBatchVertexBufferSize{ 65535 };
ShaderRender sGizmoLineBatchShader          {};
MeshGizmo    sGizmoLineBatchMesh            {};
u32          sGizmoLineBatchOffsetVertex    {};
u32          sGizmoLineBatchOffsetIndex     {};

/*
* Debug utilities.
*/

u32 CheckShaderStatus(u32 id, u32 type, std::string& log) {
  s32 compileInfo, compileInfoSize;

  glGetShaderiv(id, (GLenum)type, &compileInfo);
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &compileInfoSize);

  if (compileInfoSize <= 0) return 0;

  log.clear();
  log.resize(compileInfoSize);

  glGetShaderInfoLog(id, compileInfoSize, nullptr, &log[0]);

  return 1;
}

/*
* OpenGL context specific.
*/

void        ContextCreate(u32 width, u32 height, std::string const& title)
{
  sWidth = width;
  sHeight = height;
  sAspect = (r32)width / height;

  glfwInit();

  GLFWmonitor* pMonitor{ glfwGetPrimaryMonitor() };
  GLFWvidmode const* pMode{ glfwGetVideoMode(pMonitor) };

  //glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);
  glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  spWindow = glfwCreateWindow((s32)width, (s32)height, title.data(), nullptr, nullptr);

  glfwSetWindowCloseCallback(spWindow, [](GLFWwindow*)
    {
      sStatus = 1;
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
      if (event.mActionCurr != EventType::Down && event.mActionPrev != EventType::Held)
      {
        event.mActionCurr = EventType::Down;
      }
      else
      {
        event.mActionCurr = EventType::Held;
      }
    }

    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != EventType::Up && event.mActionPrev == EventType::Held)
      {
        event.mActionCurr = EventType::Up;
      }
      else
      {
        event.mActionCurr = EventType::None;
      }
    }
  }

  for (u32 i{}; i < GLFW_KEY_LAST; i++)
  {
    Event& event{ sKeyboardKeyStates[i] };
    s32 action{ glfwGetKey(spWindow, (s32)i) };

    if (action == GLFW_PRESS)
    {
      if (event.mActionCurr != EventType::Down && event.mActionPrev != EventType::Held)
      {
        event.mActionCurr = EventType::Down;
      }
      else
      {
        event.mActionCurr = EventType::Held;
      }
    }

    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != EventType::Up && event.mActionPrev == EventType::Held)
      {
        event.mActionCurr = EventType::Up;
      }
      else
      {
        event.mActionCurr = EventType::None;
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
  return sMouseKeyStates[key].mActionCurr == EventType::Down;
}
u32 MouseHeld(u32 key)
{
  return sMouseKeyStates[key].mActionCurr == EventType::Held;
}
u32 MouseUp(u32 key)
{
  return sMouseKeyStates[key].mActionCurr == EventType::Up;
}
u32 KeyDown(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == EventType::Down;
}
u32 KeyHeld(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == EventType::Held;
}
u32 KeyUp(u32 key)
{
  return sKeyboardKeyStates[key].mActionCurr == EventType::Up;
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
* Camera management.
*/

void CameraCreate(Camera& camera, r32v3 const& position, r32 fov, r32 near, r32 far)
{
  r32 const fovRad{ glm::radians(fov) };
  r32v3 const right{ 1.f, 0.f, 0.f };
  r32v3 const up{ 0.f, 1.f, 0.f };
  r32v3 const front{ 0.f, 0.f, -1.f };

  camera =
  {
    .mPosition  { position },
    .mFovRad    { fovRad },
    .mRight     { right },
    .mUp        { up },
    .mFront     { front },
    .mLocalRight{ right },
    .mLocalUp   { up },
    .mLocalFront{ front },
    .mNear      { near },
    .mFar       { far },
    .mProjection{ glm::perspective(fovRad, sAspect, near, far) },
    .mView      { glm::lookAt(position, position + front, up) },
  };
}
void CameraUpdateControllerInputSpace(Camera& camera, CameraControllerSpace& controller, r32 timeDelta)
{
  if (KeyHeld(GLFW_KEY_A)) controller.mPositionAccel += -glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) controller.mPositionAccel += glm::normalize(camera.mLocalRight) * controller.mPositionAmount * timeDelta;
  
  if (KeyHeld(GLFW_KEY_S)) controller.mPositionAccel += -glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) controller.mPositionAccel += glm::normalize(camera.mLocalFront) * controller.mPositionAmount * timeDelta;
  
  if (KeyHeld(GLFW_KEY_Q)) controller.mRollAccel += -controller.mRollAmount * timeDelta;
  if (KeyHeld(GLFW_KEY_E)) controller.mRollAccel += controller.mRollAmount * timeDelta;

  static r32v2 mousePositionDown{};
  
  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };
  
    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      controller.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * controller.mRotationAmount * timeDelta;
  }

  controller.mPositionVelocity += -controller.mPositionVelocity * controller.mPositionVelocityDecay * timeDelta;
  controller.mRotationVelocityAngular += -controller.mRotationVelocityAngular * controller.mRotationVelocityDecay * timeDelta;
  controller.mRollVelocityAngular += -controller.mRollVelocityAngular * controller.mRollVelocityDecay * timeDelta;
}
void CameraUpdateControllerInputOrbit(Camera& camera, CameraControllerOrbit& controller, r32 timeDelta)
{
  r32 distanceScaleLog{ glm::log(controller.mScrollDistance) };

  if (KeyHeld(GLFW_KEY_A)) controller.mPositionAccel += -glm::normalize(camera.mLocalRight) * controller.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_D)) controller.mPositionAccel += glm::normalize(camera.mLocalRight) * controller.mPositionAmount * distanceScaleLog * timeDelta;

  r32v3 mLocalFrontNoXRot{ glm::cross(camera.mUp, camera.mLocalRight) };

  if (KeyHeld(GLFW_KEY_S)) controller.mPositionAccel += -glm::normalize(mLocalFrontNoXRot) * controller.mPositionAmount * distanceScaleLog * timeDelta;
  if (KeyHeld(GLFW_KEY_W)) controller.mPositionAccel += glm::normalize(mLocalFrontNoXRot) * controller.mPositionAmount * distanceScaleLog * timeDelta;

  static r32v2 mousePositionDown{};

  if (MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mousePositionDown = { MousePositionX(), MousePositionY() };
  }
  if (MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ MousePositionX(), MousePositionY() };
    r32v2 mousePositionDelta{ mousePositionDown - mousePosition };
    r32v2 mousePositionDeltaAligned{ mousePositionDelta.x, mousePositionDelta.y };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      controller.mRotationAccel += glm::normalize(mousePositionDeltaAligned) * controller.mRotationAmount * distanceScaleLog * timeDelta;
  }

  controller.mScrollAccel += -MouseScrollY() * controller.mScrollAmount * timeDelta;

  controller.mPositionVelocity += -controller.mPositionVelocity * controller.mPositionVelocityDecay * timeDelta;
  controller.mRotationVelocityAngular += -controller.mRotationVelocityAngular * controller.mRotationVelocityDecay * timeDelta;
  controller.mScrollVelocity += -controller.mScrollVelocity * controller.mScrollVelocityDecay * timeDelta;

  controller.mScrollVelocity = glm::clamp(controller.mScrollVelocity, -2.f, 2.f);
}
void CameraUpdateControllerPhysicsSpace(Camera& camera, CameraControllerSpace& controller)
{
  controller.mPositionVelocity += controller.mPositionAccel;  
  controller.mPositionAccel = {};
  camera.mPosition += controller.mPositionVelocity;
  
  controller.mRotationVelocityAngular += controller.mRotationAccel;
  controller.mRotationAccel = {};
  if (controller.mRotationVelocityAngular.x > 180.f) controller.mRotationVelocityAngular.x = -180.f;
  if (controller.mRotationVelocityAngular.x < -180.f) controller.mRotationVelocityAngular.x = 180.f;
  if (controller.mRotationVelocityAngular.y > 180.f) controller.mRotationVelocityAngular.y = -180.f;
  if (controller.mRotationVelocityAngular.y < -180.f) controller.mRotationVelocityAngular.y = 180.f;

  controller.mRollVelocityAngular += controller.mRollAccel;
  controller.mRollAccel = {};
  if (controller.mRollVelocityAngular > 180.f) controller.mRollVelocityAngular = -180.f;
  if (controller.mRollVelocityAngular < -180.f) controller.mRollVelocityAngular = 180.f;

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.y), camera.mLocalRight);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.x), camera.mLocalUp);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRollVelocityAngular), camera.mLocalFront);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };
  
  camera.mProjection = glm::perspective(camera.mFovRad, sAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition, camera.mPosition + camera.mLocalFront, camera.mLocalUp);
}
void CameraUpdateControllerPhysicsOrbit(Camera& camera, CameraControllerOrbit& controller)
{
  controller.mPositionVelocity += controller.mPositionAccel;
  controller.mPositionAccel = {};
  camera.mPosition += controller.mPositionVelocity;

  controller.mRotationVelocityAngular += controller.mRotationAccel;
  controller.mRotationAccel = {};
  if (controller.mRotationVelocityAngular.x > 180.f) controller.mRotationVelocityAngular.x = -180.f;
  if (controller.mRotationVelocityAngular.x < -180.f) controller.mRotationVelocityAngular.x = 180.f;
  if (controller.mRotationVelocityAngular.y > 180.f) controller.mRotationVelocityAngular.y = -180.f;
  if (controller.mRotationVelocityAngular.y < -180.f) controller.mRotationVelocityAngular.y = 180.f;

  controller.mScrollVelocity += controller.mScrollAccel;
  controller.mScrollAccel = {};
  controller.mScrollDistance += controller.mScrollVelocity;
  controller.mScrollDistance = glm::clamp(controller.mScrollDistance, controller.mScrollDistanceMin, controller.mScrollDistanceMax);

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.x), camera.mUp);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocityAngular.y), camera.mLocalRight);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };

  camera.mProjection = glm::perspective(camera.mFovRad, sAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition - camera.mLocalFront * controller.mScrollDistance, camera.mPosition, camera.mLocalUp);
}

/*
* Shader management.
*/

void ShaderCreateCompute(ShaderCompute& shaderCompute, std::string const& shaderComputeSource)
{
  shaderCompute.mPid = glCreateProgram();
  shaderCompute.mCid = glCreateShader(GL_COMPUTE_SHADER);

  char const* shaderComputeSourcePtr{ shaderComputeSource.data() };

  glShaderSource(shaderCompute.mCid, 1, &shaderComputeSourcePtr, nullptr);

  std::string log{};

  glCompileShader(shaderCompute.mCid);
  if (CheckShaderStatus(shaderCompute.mCid, GL_COMPILE_STATUS, log))
    std::printf("%s\n", log.data());

  glAttachShader(shaderCompute.mPid, shaderCompute.mCid);

  glLinkProgram(shaderCompute.mPid);
  if (CheckShaderStatus(shaderCompute.mPid, GL_LINK_STATUS, log))
    std::printf("%s\n", log.data());
}
void ShaderCreateRender(ShaderRender& shaderRender, std::string const& renderShaderVertexcSource, std::string const& renderShaderFragmentSource)
{
  shaderRender.mPid = glCreateProgram();
  shaderRender.mVid = glCreateShader(GL_VERTEX_SHADER);
  shaderRender.mFid = glCreateShader(GL_FRAGMENT_SHADER);

  char const* shaderVertexSourcePtr{ renderShaderVertexcSource.data() };
  char const* shaderFragmentSourcePtr{ renderShaderFragmentSource.data() };

  glShaderSource(shaderRender.mVid, 1, &shaderVertexSourcePtr, nullptr);
  glShaderSource(shaderRender.mFid, 1, &shaderFragmentSourcePtr, nullptr);
  
  std::string log{};

  glCompileShader(shaderRender.mVid);
  if (CheckShaderStatus(shaderRender.mVid, GL_COMPILE_STATUS, log))
    std::printf("%s\n", log.data());

  glCompileShader(shaderRender.mFid);
  if (CheckShaderStatus(shaderRender.mFid, GL_COMPILE_STATUS, log))
    std::printf("%s\n", log.data());

  glAttachShader(shaderRender.mPid, shaderRender.mVid);
  glAttachShader(shaderRender.mPid, shaderRender.mFid);

  glLinkProgram(shaderRender.mPid);
  if (CheckShaderStatus(shaderRender.mPid, GL_LINK_STATUS, log))
    std::printf("%s\n", log.data());
}
void ShaderDestroyCompute(ShaderCompute const& shaderCompute)
{
  glDeleteShader(shaderCompute.mCid);
  glDeleteProgram(shaderCompute.mPid);
}
void ShaderDestroyRender(ShaderRender const& shaderRender)
{
  glDeleteShader(shaderRender.mVid);
  glDeleteShader(shaderRender.mFid);
  glDeleteProgram(shaderRender.mPid);
}
void ShaderExecuteCompute(ShaderCompute const& shaderCompute, u32 numThreadsX, u32 numThreadsY, u32 numThreadsZ)
{
  glDispatchCompute(numThreadsX, numThreadsY, numThreadsZ);
}

/*
* Model management.
*/

void ModelCreate(Model& model, std::string const& fileName)
{
  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType) };

  for (u32 i{}; i < pScene->mNumMeshes; i++)
  {
    aiMesh const* pMesh{ pScene->mMeshes[i] };

    if (!(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
      continue;

    std::vector<VertexLambert> verticesLambert{};
    std::vector<u32> indicesLambert{};

    verticesLambert.resize(pMesh->mNumVertices);
    indicesLambert.resize(pMesh->mNumFaces * 3);

    for (u32 j{}; j < pMesh->mNumVertices; j++)
    {
      verticesLambert[j] =
      {
        .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
        .mNormal  { pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
        .mColor   { 0.f, 0.f, 0.f, 1.f },
      };
    }
    for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
    {
      indicesLambert[k + 0] = { pMesh->mFaces[j].mIndices[0] };
      indicesLambert[k + 1] = { pMesh->mFaces[j].mIndices[1] };
      indicesLambert[k + 2] = { pMesh->mFaces[j].mIndices[2] };
    }

    MeshLambert meshLambert{};

    MeshLayoutCreate(meshLambert, (u32)verticesLambert.size(), (u32)indicesLambert.size());
    MeshLayoutBind(meshLambert);
    MeshLayoutData(meshLambert, verticesLambert.data(), indicesLambert.data(), (u32)verticesLambert.size(), (u32)indicesLambert.size());
    MeshLayoutUnbind(meshLambert);

    model.mModel.mMeshes.emplace_back(meshLambert);
  }
  for (u32 i{}; i < pScene->mNumMaterials; i++)
  {
    aiMaterial const* pMaterial{ pScene->mMaterials[i] };

    for (u32 j{}; j < pMaterial->mNumProperties; j++)
    {
      //std::printf("%s\n", pMaterial->mProperties[j]->mKey.data);
    }
  }
  for (u32 i{}; i < pScene->mNumTextures; i++)
  {
    aiTexture const* pTexture{ pScene->mTextures[i] };
  }

  model.mTransform = glm::identity<r32m4>();
}
void ModelRender(Model const& model)
{
  for (auto const& mesh : model.mModel.mMeshes)
  {
    MeshLayoutBind(mesh);
    MeshLayoutRender(mesh, RenderMode::Triangles);
    MeshLayoutUnbind(mesh);
  }
}
void ModelDestroy(Model const& model)
{
  for (auto const& mesh : model.mModel.mMeshes)
    MeshLayoutDestroy(mesh);
}

/*
* 3D debug utilities.
*/

void GizmoLineBatchCreate()
{
  ShaderCreateRender(sGizmoLineBatchShader, sRenderShaderVertexGizmo, sRenderShaderFragmentGizmo);
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
  VertexGizmoLine vertices[2]{ { p0, color }, { p1, color } };
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

  VertexGizmoLine vertices[8]
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
void GizmoLineBatchUnbind()
{
  MeshLayoutUnbind(sGizmoLineBatchMesh);
}
void GizmoLineBatchRender()
{
  ShaderBind(sGizmoLineBatchShader);
  ShaderUniformR32M4(sGizmoLineBatchShader, "uProjection", SceneActive()->mCamera.mProjection);
  ShaderUniformR32M4(sGizmoLineBatchShader, "uView", SceneActive()->mCamera.mView);
  MeshLayoutRender(sGizmoLineBatchMesh, RenderMode::Lines);

  sGizmoLineBatchOffsetVertex = 0;
  sGizmoLineBatchOffsetIndex = 0;
}