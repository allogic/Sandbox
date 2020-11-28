#pragma once

#include <Core.h>
#include <Types.h>

#include <Components/TextureComponents.h>
#include <Components/VertexComponents.h>

/*
* Texture loading utilities.
*/

template<typename TextureLayout> void TextureFrom(TextureLayout& textureLayout, std::string const& fileName)
{
  u32 width{};
  u32 height{};
  u32 channels{};
  u32 type{};

  switch (TextureLayout::sFormat)
  {
    case eTextureFormatRGB: type = STBI_rgb; break;
    case eTextureFormatRGBA: type = STBI_rgb_alpha; break;
  }

  u8* pBlob = stbi_load(fileName.data(), (s32*)&width, (s32*)&height, (s32*)&channels, (s32)type);

  assert(pBlob);
  assert(width > 0 && width % 2 == 0);
  assert(height > 0 && height % 2 == 0);

  switch (TextureLayout::sFormat)
  {
    case eTextureFormatRGB: assert(channels == 3); break;
    case eTextureFormatRGBA: assert(channels == 4); break;
  }

  TextureLayoutCreate(textureLayout, width, height, 0);
  TextureLayoutBind(textureLayout);

  if (TextureLayout::sLayout == eTextureLayoutR32)
  {
    std::vector<r32> imageData{};

    imageData.resize(width * height * channels);

    for (u32 i{}; i < imageData.size(); i++)
      imageData[i] = pBlob[i] / 255.f;

    TextureLayoutDataSet(textureLayout, 0, imageData.data());
  }
  else
  {
    TextureLayoutDataSet(textureLayout, 0, pBlob);
  }

  stbi_image_free(pBlob);

  TextureLayoutUnbind();
}

/*
* Mesh loading utilities.
*/

template<typename MeshLayout> void MeshFrom(MeshLayout& meshLayout, std::string const& fileName)
{
  static_assert(MeshLayout::VertexType::sType == eVertexLayoutLambert);

  Assimp::Importer importer{};

  aiScene const* pScene{ importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_SortByPType) };

  assert(pScene);
  assert(pScene->mNumMeshes > 0);
  assert(pScene->mNumMaterials > 0);

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
      indexBuffers[i][k + 0] = pMesh->mFaces[j].mIndices[0];
      indexBuffers[i][k + 1] = pMesh->mFaces[j].mIndices[1];
      indexBuffers[i][k + 2] = pMesh->mFaces[j].mIndices[2];
    }
  }
  for (u32 i{}; i < numMaterials; i++)
  {
    aiMaterial const* pMaterial{ pScene->mMaterials[i] };

    for (u32 i{ aiTextureType_DIFFUSE }; i < aiTextureType_UNKNOWN; i++)
    {
      aiString texturePath{};
      u32 const textureCount{ pMaterial->GetTextureCount((aiTextureType)i) };

      for (u32 j{}; j < textureCount; j++)
      {
        pMaterial->GetTexture((aiTextureType)i, j, &texturePath);
        std::printf("%s\n", texturePath.C_Str());
      }
    }

    //for (u32 j{}; j < pMaterial->mNumProperties; j++)
    //{
    //  std::printf("%s\n", pMaterial->mProperties[j]->mKey.data);
    //}
  }
  for (u32 i{}; i < numTextures; i++)
  {
    aiTexture const* pTexture{ pScene->mTextures[i] };
  }

  MeshLayoutCreate(meshLayout, numMeshes, vertexBufferSizes.data(), indexBufferSizes.data());

  for (u32 i{}; i < numMeshes; i++)
  {
    MeshLayoutBind(meshLayout, i);
    MeshLayoutData(meshLayout, i, vertexBuffers[i].data(), indexBuffers[i].data());
  }

  MeshLayoutUnbind();
}