#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

std::vector<Scene*> sScenes      {};
Scene*              spSceneActive{};

s32 sStatus{};
u32 sWidth { 1280 };
u32 sHeight{ 720 };
r32 sAspect{ (r32)sWidth / sHeight };

MouseMoveClb   sOnMouseMove  {};
MouseButtonClb sOnMouseButton{};
WindowCloseClb sOnWindowClose{};
KeyboardClb    sOnKeyboard   {};

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

void ContextCreate(GLFWwindow*& pHandle, u32 width, u32 height, std::string const& title)
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  pHandle = glfwCreateWindow((s32)width, (s32)height, title.data(), nullptr, nullptr);

  glfwSetWindowCloseCallback(pHandle, sOnWindowClose);
  glfwSetCursorPosCallback(pHandle, sOnMouseMove);
  glfwSetMouseButtonCallback(pHandle, sOnMouseButton);
  glfwSetKeyCallback(pHandle, sOnKeyboard);

  glfwMakeContextCurrent(pHandle);
  glfwSwapInterval(0);

  gladLoadGL();
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
    spSceneActive->OnEnable(sAspect);
  }
}
void SceneSwitch(u32 index)
{
  spSceneActive->OnDisable();
  spSceneActive = sScenes[index];
  spSceneActive->OnEnable(sAspect);
}
void SceneActive(Scene*& pScene)
{
  pScene = spSceneActive;
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

void CameraCreate(Camera& camera, r32v3 const& position, r32 fov, r32 aspect, r32 near, r32 far)
{
  r32 const fovRad{ glm::radians(fov) };
  r32v3 const right{ 1.f, 0.f, 0.f };
  r32v3 const up{ 0.f, 1.f, 0.f };
  r32v3 const front{ 0.f, 0.f, 1.f };

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
    .mProjection{ glm::perspective(fovRad, aspect, near, far) },
    .mView      { glm::lookAt(position, position + front, up) },
  };
}
void CameraUpdateController(Camera& camera, CameraControllerOrbit& controller, r32 timeDelta)
{
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
void ShaderDestroy(Shader const& shader)
{
  glDeleteShader(shader.mVid);
  glDeleteShader(shader.mFid);
  glDeleteProgram(shader.mPid);
}

/*
* Geometry management.
*/

void MeshCreate(Mesh& mesh, std::vector<r32> const& vertexLayout, u32 numVertexElements, std::vector<u32> const& indices)
{
  mesh =
  {
    .mNumVertices{ (u32)vertexLayout.size() / numVertexElements },
    .mNumIndices { (u32)indices.size() },
  };

  glGenVertexArrays(1, &mesh.mVao);
  glGenBuffers(1, &mesh.mVbo);
  glGenBuffers(1, &mesh.mEbo);

  glBindVertexArray(mesh.mVao);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexLayout.size() * sizeof(r32), vertexLayout.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(r32), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(r32), (void*)(3 * sizeof(r32)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}
void MeshDestroy(Mesh const& mesh)
{
  glDeleteBuffers(1, &mesh.mVbo);
  glDeleteBuffers(1, &mesh.mEbo);
  glDeleteVertexArrays(1, &mesh.mVao);

  delete[] mesh.mpVertices;
  delete[] mesh.mpIndices;
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

    Mesh mesh
    {
      .mNumVertices{ pMesh->mNumVertices },
      .mNumIndices { pMesh->mNumFaces * 3 },
      .mpVertices  { new Vertex[pMesh->mNumVertices] },
      .mpIndices   { new u32[pMesh->mNumFaces * 3] },
    };

    glGenVertexArrays(1, &mesh.mVao);
    glGenBuffers(1, &mesh.mVbo);
    glGenBuffers(1, &mesh.mEbo);

    glBindVertexArray(mesh.mVao);

    for (u32 j{}; j < pMesh->mNumVertices; j++)
    {
      mesh.mpVertices[j] =
      {
        .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
        .mNormal{ pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
      };
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.mNumVertices * sizeof(Vertex), mesh.mpVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(r32v3)));

    for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
    {
      mesh.mpIndices[k + 0] = { pMesh->mFaces[j].mIndices[0] };
      mesh.mpIndices[k + 1] = { pMesh->mFaces[j].mIndices[1] };
      mesh.mpIndices[k + 2] = { pMesh->mFaces[j].mIndices[2] };
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.mNumIndices * sizeof(u32), mesh.mpIndices, GL_STATIC_DRAW);

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
void ModelDestroy(Model const& model)
{
  for (auto const& mesh : model.mMeshes)
    MeshDestroy(mesh);
}

void ApplyUniformMat4(Shader const& shader, std::string const& name, r32m4 const& matrix)
{
  u32 id{ (u32)glGetUniformLocation(shader.mPid, name.data()) };
  glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]);
}

void BindShader(Shader const& shader)
{
  glUseProgram(shader.mPid);
}
void BindMesh(Mesh const& mesh)
{
  glBindVertexArray(mesh.mVao);
  glDrawElements(GL_TRIANGLES, mesh.mNumIndices, GL_UNSIGNED_INT, nullptr);
}
void BindModel(Model const& model)
{
  for (auto const& mesh : model.mMeshes)
  {
    glBindVertexArray(mesh.mVao);
    glDrawElements(GL_TRIANGLES, mesh.mNumIndices, GL_UNSIGNED_INT, nullptr); // use glDrawArrays here
  }
}

void LineBatchBegin()
{

}
void LinePush(r32v3 const& p0, r32v3 const& p1, r32v3 const& c0, r32v3 const& c1)
{

}
void LineBatchEnd()
{

}
void LineBatchRender(Shader const& shader)
{

}