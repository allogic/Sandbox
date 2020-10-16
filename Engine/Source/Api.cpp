#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

u32 status{};

void CreateShaderProgram()
{
  
}

void CreateVertexLayout()
{
  
}

void CreateWindow(Window& window, u32 width, u32 height, std::string const& title)
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  window.mpHdl = glfwCreateWindow((s32)width, (s32)height, title.data(), nullptr, nullptr);

  glfwSetWindowCloseCallback(window.mpHdl, [](GLFWwindow* pHdl) { status = 1; });

  glfwMakeContextCurrent(window.mpHdl);
  glfwSwapInterval(0);

  gladLoadGL();
}

void CreateShader(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource)
{
  shader.mPid = glCreateProgram();
  shader.mVid = glCreateShader(GL_VERTEX_SHADER);
  shader.mFid = glCreateShader(GL_FRAGMENT_SHADER);

  char const* vertexShaderSourcePtr{ vertexShaderSource.data() };
  char const* fragmentShaderSourcePtr{ fragmentShaderSource.data() };

  glShaderSource(shader.mVid, 1, &vertexShaderSourcePtr, nullptr);
  glShaderSource(shader.mFid, 1, &fragmentShaderSourcePtr, nullptr);
  
  glCompileShader(shader.mVid);
  glCompileShader(shader.mFid);

  glAttachShader(shader.mPid, shader.mVid);
  glAttachShader(shader.mPid, shader.mFid);

  glLinkProgram(shader.mPid);
}

void CreateModel(Model& model, std::string const& fileName)
{
  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, aiProcess_Triangulate) };

  struct Vertex
  {
    r32v3 mPosition{};
    r32v3 mNormal  {};
  };

  std::vector<Vertex> vertices{};
  std::vector<u32> indices{};

  // read meshes
  for (u32 i{}; i < pScene->mNumMeshes; i++)
  {
    aiMesh const* pMesh{ pScene->mMeshes[i] };

    Mesh mesh{};

    glGenVertexArrays(1, &mesh.mVao);
    glGenBuffers(1, &mesh.mVbo);
    glGenBuffers(1, &mesh.mEbo);

    glBindVertexArray(mesh.mVao);

    vertices.clear();
    vertices.resize(pMesh->mNumVertices);
    for (u32 j{}; j < pMesh->mNumVertices; j++)
    {
      vertices[j] = {
        .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
        .mNormal{ pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
      };
    }

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(r32v3), nullptr);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(r32v3), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.mVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    indices.clear();
    indices.resize(pMesh->mNumFaces * 3);
    for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
    {
      indices[k + 0] = { pMesh->mFaces[j].mIndices[0] };
      indices[k + 1] = { pMesh->mFaces[j].mIndices[1] };
      indices[k + 2] = { pMesh->mFaces[j].mIndices[2] };
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    model.mMeshes.emplace_back(mesh);
  }

  // read materials
  for (u32 i{}; i < pScene->mNumMaterials; i++)
  {
    aiMaterial const* pMaterial{ pScene->mMaterials[i] };

    for (u32 j{}; j < pMaterial->mNumProperties; j++)
    {
      std::printf("%s\n", pMaterial->mProperties[j]->mKey.data);
    }
  }

  // read textures
  for (u32 i{}; i < pScene->mNumTextures; i++)
  {
    aiTexture const* pTexture{ pScene->mTextures[i] };
  }
}