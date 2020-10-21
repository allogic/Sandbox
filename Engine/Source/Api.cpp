#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

GLFWwindow* spWindow{};

std::vector<Scene*> sScenes      {};
Scene*              spSceneActive{};

s32 sStatus{};
u32 sWidth {};
u32 sHeight{};
r32 sAspect{};

r32v2 sMousePosition{};

Shader sShaderLineBatch{};
Mesh   sMeshLineBatch  {};

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

void ContextCreate(u32 width, u32 height, std::string const& title)
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
  glfwSetCursorPosCallback(spWindow, [](GLFWwindow*, r64 x, r64 y)
  {
    sMousePosition = { (r32)x, (r32)y };
  });
  glfwSetMouseButtonCallback(spWindow, [](GLFWwindow*, s32 button, s32 action, s32 mods)
  {
    
  });
  glfwSetKeyCallback(spWindow, [](GLFWwindow*, s32 key, s32 scancode, s32 action, s32 mods)
  {
    
  });

  glfwMakeContextCurrent(spWindow);
  glfwSwapInterval(0);

  gladLoadGL();
}
void ContextDestroy()
{
  glfwDestroyWindow(spWindow);
}

/*
* Mouse/Keyboard controls.
*/

void MouseDown()
{

}
void MouseHeld()
{

}
void MouseUp()
{

}
void KeyDown()
{

}
void KeyHeld()
{

}
void KeyUp()
{

}

/*
* Scene management.
*/

void SceneCreate(Scene* pScene)
{
  sScenes.emplace_back(pScene);

  if (!spSceneActive)
  {
    spSceneActive = sScenes.back();
    spSceneActive->OnEnable();
  }
}
void SceneSwitch(u32 index)
{
  spSceneActive->OnDisable();
  spSceneActive = sScenes[index];
  spSceneActive->OnEnable();
}
void SceneDestroyAll()
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
void CameraUpdateController(Camera& camera, CameraControllerOrbit& controller, r32 timeDelta)
{
  if (key == GLFW_KEY_A) sCamera.mPosition += sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;
  if (key == GLFW_KEY_D) sCamera.mPosition += -sCamera.mLocalRight * sCamera.mPositionSpeed * sTimeDelta;
  
  if (key == GLFW_KEY_S) sCamera.mPosition += -sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;
  if (key == GLFW_KEY_W) sCamera.mPosition += sCamera.mLocalFront * sCamera.mPositionSpeed * sTimeDelta;

  //r32v2 mousePosition{ (r32)x, (r32)y };
  //r32v2 mousePositionDelta{ r32v2{ (r32)sWidth / 2, (r32)sHeight / 2 } - mousePosition };
  //sCamera.mRotationDrag = sCamera.mLockDrag ? r32v2{} : r32v2{ mousePositionDelta.x, -mousePositionDelta.y };

  //if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) sCamera.mLockDrag = 0;
  //else                                                                                      sCamera.mLockDrag = 1;

  if (glm::length(controller.mRotationVelocity) > controller.mRotationDeadzone)
    controller.mRotationVelocity += -controller.mRotationVelocity * controller.mRotationDecay * timeDelta;
  else
    controller.mRotationVelocity = { 0.f, 0.f };

  controller.mRotationVelocity += controller.mRotationDrag * controller.mRotationSpeed * timeDelta;

  if (controller.mRotationVelocity.x > 180.f) controller.mRotationVelocity.x = -180.f;
  if (controller.mRotationVelocity.x < -180.f) controller.mRotationVelocity.x = 180.f;

  if (controller.mRotationVelocity.y > 180.f) controller.mRotationVelocity.y = -180.f;
  if (controller.mRotationVelocity.y < -180.f) controller.mRotationVelocity.y = 180.f;

  r32m4 localRotation{ glm::identity<r32m4>() };
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocity.y), camera.mLocalRight);
  localRotation = glm::rotate(localRotation, glm::radians(controller.mRotationVelocity.x), camera.mLocalUp);

  camera.mLocalRight = localRotation * r32v4{ camera.mLocalRight, 1.f };
  camera.mLocalUp = localRotation * r32v4{ camera.mLocalUp, 1.f };
  camera.mLocalFront = localRotation * r32v4{ camera.mLocalFront, 1.f };

  camera.mProjection = glm::perspective(camera.mFovRad, sAspect, camera.mNear, camera.mFar);
  camera.mView = glm::lookAt(camera.mPosition, camera.mPosition + camera.mLocalFront, camera.mLocalUp);
}

/*
* Shader management.
*/

void ShaderCreate(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource)
{
  shader.mPid = glCreateProgram();
  shader.mVid = glCreateShader(GL_VERTEX_SHADER);
  shader.mFid = glCreateShader(GL_FRAGMENT_SHADER);

  char const* vertexShaderSourcePtr{ vertexShaderSource.data() };
  char const* fragmentShaderSourcePtr{ fragmentShaderSource.data() };

  glShaderSource(shader.mVid, 1, &vertexShaderSourcePtr, nullptr);
  glShaderSource(shader.mFid, 1, &fragmentShaderSourcePtr, nullptr);
  
  std::string log{};

  glCompileShader(shader.mVid);
  if (CheckShaderStatus(shader.mVid, GL_COMPILE_STATUS, log))
    std::printf("%s\n", log.data());

  glCompileShader(shader.mFid);
  if (CheckShaderStatus(shader.mFid, GL_COMPILE_STATUS, log))
    std::printf("%s\n", log.data());

  glAttachShader(shader.mPid, shader.mVid);
  glAttachShader(shader.mPid, shader.mFid);

  glLinkProgram(shader.mPid);
  if (CheckShaderStatus(shader.mFid, GL_LINK_STATUS, log))
    std::printf("%s\n", log.data());
}
void ShaderBind(Shader const& shader)
{
  glUseProgram(shader.mPid);
}
void ShaderDestroy(Shader const& shader)
{
  glDeleteShader(shader.mVid);
  glDeleteShader(shader.mFid);
  glDeleteProgram(shader.mPid);
}
void ShaderUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix)
{
  u32 id{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]);
}

/*
* Geometry management.
*/

void MeshCreate(Mesh& mesh, std::vector<Vertex> const& vertices, std::vector<u32> const& indices)
{
  mesh.mVertices = vertices;
  mesh.mIndices = indices;

  glGenVertexArrays(1, &mesh.mVao);
  glGenBuffers(1, &mesh.mVbo);
  glGenBuffers(1, &mesh.mEbo);

  glBindVertexArray(mesh.mVao);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
  glBufferData(GL_ARRAY_BUFFER, mesh.mVertices.size() * sizeof(Vertex), mesh.mVertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(r32v3)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(r32v3)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.mIndices.size() * sizeof(u32), mesh.mIndices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}
void MeshBind(Mesh const& mesh)
{
  glBindVertexArray(mesh.mVao);
  glDrawElements(GL_TRIANGLES, (s32)mesh.mIndices.size(), GL_UNSIGNED_INT, nullptr);
}
void MeshDestroy(Mesh const& mesh)
{
  glDeleteBuffers(1, &mesh.mVbo);
  glDeleteBuffers(1, &mesh.mEbo);
  glDeleteVertexArrays(1, &mesh.mVao);
}
void ModelCreate(Model& model, std::string const& fileName)
{
  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType) };

  for (u32 i{}; i < pScene->mNumMeshes; i++)
  {
    aiMesh const* pMesh{ pScene->mMeshes[i] };

    if (!(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
      continue;

    Mesh mesh{};
    mesh.mVertices.resize(pMesh->mNumVertices);
    mesh.mIndices.resize(pMesh->mNumFaces * 3);

    glGenVertexArrays(1, &mesh.mVao);
    glGenBuffers(1, &mesh.mVbo);
    glGenBuffers(1, &mesh.mEbo);

    glBindVertexArray(mesh.mVao);

    for (u32 j{}; j < pMesh->mNumVertices; j++)
    {
      mesh.mVertices[j] =
      {
        .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
        .mNormal  { pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
        .mColor   { 0.f, 0.f, 0.f, 1.f },
      };
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.mVertices.size() * sizeof(Vertex), mesh.mVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(r32v3)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(r32v3)));

    for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
    {
      mesh.mIndices[k + 0] = { pMesh->mFaces[j].mIndices[0] };
      mesh.mIndices[k + 1] = { pMesh->mFaces[j].mIndices[1] };
      mesh.mIndices[k + 2] = { pMesh->mFaces[j].mIndices[2] };
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.mIndices.size() * sizeof(u32), mesh.mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    model.mMeshes.emplace_back(mesh);
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
void ModelBind(Model const& model)
{
  for (auto const& mesh : model.mMeshes)
  {
    glBindVertexArray(mesh.mVao);
    glDrawElements(GL_TRIANGLES, (s32)mesh.mIndices.size(), GL_UNSIGNED_INT, nullptr);
  }
}
void ModelDestroy(Model const& model)
{
  for (auto const& mesh : model.mMeshes)
    MeshDestroy(mesh);
}

/*
* 3D debug utilities.
*/

void LineBatchCreate()
{
  ShaderCreate(sShaderLineBatch, sVertexShaderSourceLine, sFragmentShaderSourceLine);

  glGenVertexArrays(1, &sMeshLineBatch.mVao);
  glGenBuffers(1, &sMeshLineBatch.mVbo);
  glGenBuffers(1, &sMeshLineBatch.mEbo);
}
void LineBatchBegin()
{
  sMeshLineBatch.mVertices.clear();
  sMeshLineBatch.mIndices.clear();
}
void LineBatchPush(r32v3 const& p0, r32v3 const& p1, r32v4 const& c0, r32v4 const& c1)
{
  u32 const numVertices{ (u32)sMeshLineBatch.mVertices.size() };

  sMeshLineBatch.mVertices.emplace_back(Vertex{ p0, r32v3{}, c0 });
  sMeshLineBatch.mVertices.emplace_back(Vertex{ p1, r32v3{}, c1 });

  sMeshLineBatch.mIndices.emplace_back(numVertices + 0);
  sMeshLineBatch.mIndices.emplace_back(numVertices + 1);
}
void LineBatchEnd()
{
  glBindVertexArray(sMeshLineBatch.mVao);

  glBindBuffer(GL_ARRAY_BUFFER, sMeshLineBatch.mVbo);
  glBufferData(GL_ARRAY_BUFFER, sMeshLineBatch.mVertices.size() * sizeof(Vertex), sMeshLineBatch.mVertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(r32v3)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sMeshLineBatch.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sMeshLineBatch.mIndices.size() * sizeof(u32), sMeshLineBatch.mIndices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}
void LineBatchRender()
{
  ShaderBind(sShaderLineBatch);

  ShaderUniformMat4(sShaderLineBatch, "uProjection", spSceneActive->mCamera.mProjection);
  ShaderUniformMat4(sShaderLineBatch, "uView", spSceneActive->mCamera.mView);

  glBindVertexArray(sMeshLineBatch.mVao);
  glDrawElements(GL_LINES, (s32)sMeshLineBatch.mIndices.size(), GL_UNSIGNED_INT, nullptr);
}
void LineBatchDestroy()
{
  MeshDestroy(sMeshLineBatch);
  ShaderDestroy(sShaderLineBatch);
}