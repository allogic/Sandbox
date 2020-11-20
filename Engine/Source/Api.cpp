#include <Api.h>

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