#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

void CreateShaderProgram()
{
  
}

void CreateVertexLayout()
{
  
}

void CreateWindow(GLFWwindow*& pWindow, u32 width, u32 height, std::string const& title)
{
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  pWindow = glfwCreateWindow((s32)width, (s32)height, title.data(), nullptr, nullptr);

  glfwMakeContextCurrent(pWindow);
  glfwSwapInterval(0);

  gladLoadGL();
}

void CreateShader(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource)
{
  
}

void CreateModel(Mesh& mesh, Material& material, std::string const& fileName)
{
  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, 0) };
  aiMesh const* pMesh{ pScene->mMeshes[0] };

  mesh = { pMesh->mNumVertices, pMesh->mNumFaces * 3 };
  material = {};

  for (u32 i{}; i < pMesh->mNumVertices; i++)
  {
    mesh.mpVertices[i] = {
      .mPosition{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
      .mNormal  { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z },
    };
  }

  for (u32 i{}, j{}; i < pMesh->mNumFaces; i++, j += 3)
  {
    mesh.mpIndices[j + 0] = { pMesh->mFaces[i].mIndices[0] };
    mesh.mpIndices[j + 1] = { pMesh->mFaces[i].mIndices[1] };
    mesh.mpIndices[j + 2] = { pMesh->mFaces[i].mIndices[2] };
  }
}