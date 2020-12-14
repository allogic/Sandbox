#pragma once

#include <Core.h>
#include <Types.h>
#include <Importer.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/TextureLayouts.h>

/*
* Loadable layouts.
*/

enum AsyncState : u32
{
  eAsyncStateNone,
  eAsyncStateLoaded,
  eAsyncStateReady,
};

template<typename Type>
struct Loadable
{
  AsyncState mStatus  { eAsyncStateNone };
  Type       mInstance{};
};

template<typename MeshLayout>
struct LoadableMesh : Loadable<MeshLayout>
{
  u32    mNumSubMeshes      {};
  u32*   mpVertexBufferSizes{};
  u32*   mpIndexBufferSizes {};
  void** mppVertexBuffers   {};
  void** mppIndexBuffers    {};
};

template<typename TextureLayout>
struct LoadableTexture : Loadable<TextureLayout>
{
  u32   mWidth {};
  u32   mHeight{};
  void* mpData {};
};

static                           std::mutex                                            sMutexDb       {};
template<typename MeshLayout>    std::map<std::string, LoadableMesh<MeshLayout>>       sAsyncMeshDb   {};
template<typename TextureLayout> std::map<std::string, LoadableTexture<TextureLayout>> sAsyncTextureDb{};

/*
* Async asset database.
*/

namespace ADB
{
  /*
  * Async data importer.
  */

  template<typename MeshLayout>    void LoadMeshAsync(LoadableMesh<MeshLayout>* pLoadable, std::string file)
  {
    static_assert(MeshLayout::VertexType::sType == eVertexLayoutLambert);

    sMutexDb.lock();
    std::cout << "Mesh Thread[" << std::this_thread::get_id() << "] Started" << std::endl;
    SANDBOX_ENGINE_MEASURE_BEGIN(LoadMeshAsync)
    sMutexDb.unlock();

    Assimp::Importer importer{};

    aiScene const* pScene
    {
      importer.ReadFile(
        file,
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipUVs |
        aiProcess_FlipWindingOrder
      )
    };

    assert(pScene);
    assert(pScene->mNumMeshes > 0);
    assert(pScene->mNumMaterials > 0);

    pLoadable->mNumSubMeshes = pScene->mNumMeshes;
    pLoadable->mpVertexBufferSizes = new u32[pScene->mNumMeshes];
    pLoadable->mpIndexBufferSizes = new u32[pScene->mNumMeshes];
    pLoadable->mppVertexBuffers = new void*[pScene->mNumMeshes];
    pLoadable->mppIndexBuffers = new void*[pScene->mNumMeshes];

    for (u32 i{}; i < pScene->mNumMeshes; i++)
    {
      aiMesh const* pMesh{ pScene->mMeshes[i] };

      if (!(pMesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
        continue;

      pLoadable->mpVertexBufferSizes[i] = pMesh->mNumVertices;
      pLoadable->mpIndexBufferSizes[i] = pMesh->mNumFaces * 3;

      pLoadable->mppVertexBuffers[i] = new typename MeshLayout::VertexType[pMesh->mNumVertices];
      pLoadable->mppIndexBuffers[i] = new u32[pMesh->mNumFaces * 3];

      for (u32 j{}; j < pMesh->mNumVertices; j++)
      {
        ((typename MeshLayout::VertexType*)(pLoadable->mppVertexBuffers[i]))[j] =
        {
          .mPosition{ pMesh->mVertices[j].x, pMesh->mVertices[j].y, pMesh->mVertices[j].z },
          .mNormal  { pMesh->mNormals[j].x, pMesh->mNormals[j].y, pMesh->mNormals[j].z },
          .mUv      { pMesh->mTextureCoords[0][j].x, pMesh->mTextureCoords[0][j].y },
          .mColor   { 0.f, 0.f, 0.f, 1.f },
        };
      }
      for (u32 j{}, k{}; j < pMesh->mNumFaces; j++, k += 3)
      {
        ((u32*)(pLoadable->mppIndexBuffers[i]))[k + 0] = pMesh->mFaces[j].mIndices[0];
        ((u32*)(pLoadable->mppIndexBuffers[i]))[k + 1] = pMesh->mFaces[j].mIndices[1];
        ((u32*)(pLoadable->mppIndexBuffers[i]))[k + 2] = pMesh->mFaces[j].mIndices[2];
      }
    }
    for (u32 i{}; i < pScene->mNumMaterials; i++)
    {
      aiMaterial const* pMaterial{ pScene->mMaterials[i] };

      for (u32 i{ aiTextureType_DIFFUSE }; i < aiTextureType_UNKNOWN; i++)
      {
        aiString texturePath{};
        u32 const textureCount{ pMaterial->GetTextureCount((aiTextureType)i) };

        for (u32 j{}; j < textureCount; j++)
        {
          pMaterial->GetTexture((aiTextureType)i, j, &texturePath);
          //std::printf("%s\n", texturePath.C_Str());
        }
      }

      //for (u32 j{}; j < pMaterial->mNumProperties; j++)
      //{
      //  std::printf("%s\n", pMaterial->mProperties[j]->mKey.data);
      //}
    }
    for (u32 i{}; i < pScene->mNumTextures; i++)
    {
      aiTexture const* pTexture{ pScene->mTextures[i] };
    }

    pLoadable->mStatus = eAsyncStateLoaded;

    sMutexDb.lock();
    std::cout << "Mesh Thread[" << std::this_thread::get_id() << "] Finished" << std::endl;
    SANDBOX_ENGINE_MEASURE_END(LoadMeshAsync)
    sMutexDb.unlock();

    while (pLoadable->mStatus == eAsyncStateLoaded) {}

    delete[] pLoadable->mpVertexBufferSizes;
    delete[] pLoadable->mpIndexBufferSizes;

    for (u32 i{}; i < pLoadable->mNumSubMeshes; i++)
      delete[] pLoadable->mppVertexBuffers[i];

    for (u32 i{}; i < pLoadable->mNumSubMeshes; i++)
      delete[] pLoadable->mppIndexBuffers[i];

    delete[] pLoadable->mppVertexBuffers;
    delete[] pLoadable->mppIndexBuffers;
  }
  template<typename TextureLayout> void LoadTextureAsync(LoadableTexture<TextureLayout>* pLoadable, std::string file)
  {
    sMutexDb.lock();
    std::cout << "Texture Thread[" << std::this_thread::get_id() << "] Started" << std::endl;
    SANDBOX_ENGINE_MEASURE_BEGIN(LoadTextureAsync)
    sMutexDb.unlock();

    u32 width{};
    u32 height{};
    u32 channels{};
    u32 type{};

    switch (TextureLayout::sFormat)
    {
      case eTextureFormatRGB: type = STBI_rgb; break;
      case eTextureFormatRGBA: type = STBI_rgb_alpha; break;
    }

    u8* pBlob = stbi_load(
      file.c_str(),
      (s32*)&width,
      (s32*)&height,
      (s32*)&channels,
      (s32)type
    );

    assert(pBlob);
    assert(width > 0 && width % 2 == 0);
    assert(height > 0 && height % 2 == 0);

    switch (TextureLayout::sFormat)
    {
      case eTextureFormatRGB: assert(channels == 3); break;
      case eTextureFormatRGBA: assert(channels == 4); break;
    }

    u32 const imageSize{ width * height * channels };

    pLoadable->mpData = new r32[imageSize];

    for (u32 i{}; i < imageSize; i++)
      ((r32*)pLoadable->mpData)[i] = pBlob[i] / 255.f;

    pLoadable->mWidth = width;
    pLoadable->mHeight = height;
    pLoadable->mStatus = eAsyncStateLoaded;

    stbi_image_free(pBlob);

    sMutexDb.lock();
    std::cout << "Texture Thread[" << std::this_thread::get_id() << "] Finished" << std::endl;
    SANDBOX_ENGINE_MEASURE_END(LoadTextureAsync)
    sMutexDb.unlock();

    while (pLoadable->mStatus == eAsyncStateLoaded) {}

    delete[] pLoadable->mpData;
  }

  /*
  * Access/queuing mechanism.
  */

  template<typename MeshLayout>    Loadable<MeshLayout>*    Mesh(std::string const& file)
  {
    auto const meshIt{ sAsyncMeshDb<MeshLayout>.find(file) };

    if (meshIt == sAsyncMeshDb<MeshLayout>.end())
    {
      auto const [insertIt, _] { sAsyncMeshDb<MeshLayout>.emplace(file, LoadableMesh<MeshLayout>{}) };

      MeshFrom(insertIt->second.mInstance, SANDBOX_ROOT_PATH "Models\\" + file);
      insertIt->second.mStatus = eAsyncStateReady;

      //std::thread
      //{
      //  &LoadMeshAsync<MeshLambert>,
      //  &insertIt->second,
      //  SANDBOX_ROOT_PATH "Models\\" + file,
      //}.detach();

      return (Loadable<MeshLayout>*)&insertIt->second;
    }

    return (Loadable<MeshLayout>*)&meshIt->second;
  }
  template<typename TextureLayout> Loadable<TextureLayout>* Texture(std::string const& file)
  {
    auto const textureIt{ sAsyncTextureDb<TextureLayout>.find(file) };

    if (textureIt == sAsyncTextureDb<TextureLayout>.end())
    {
      auto const [insertIt, _] { sAsyncTextureDb<TextureLayout>.emplace(file, LoadableTexture<TextureLayout>{}) };

      TextureFrom(insertIt->second.mInstance, SANDBOX_ROOT_PATH "Textures\\" + file);
      insertIt->second.mStatus = eAsyncStateReady;

      //std::thread
      //{
      //  &LoadTextureAsync<TextureLayout>,
      //  &insertIt->second,
      //  SANDBOX_ROOT_PATH "Textures\\" + file,
      //}.detach();

      return (Loadable<TextureLayout>*)&insertIt->second;
    }

    return (Loadable<TextureLayout>*)&textureIt->second;
  }

  /*
  * Internal state management.
  */

  static void Update()
  {
    for (auto& [_, loadable] : sAsyncMeshDb<MeshLambert>)
    {
      if (loadable.mStatus == eAsyncStateLoaded)
      {
        MeshLayoutCreate(loadable.mInstance, loadable.mNumSubMeshes, loadable.mpVertexBufferSizes, loadable.mpIndexBufferSizes);
        for (u32 i{}; i < loadable.mInstance.mNumSubMeshes; i++)
        {
          MeshLayoutBind(loadable.mInstance, i);
          MeshLayoutData(loadable.mInstance, i, loadable.mppVertexBuffers[i], loadable.mppIndexBuffers[i]);
        }
        MeshLayoutUnbind();

        loadable.mStatus = eAsyncStateReady;
      }
    }
    for (auto& [_, loadable] : sAsyncTextureDb<TextureR32RGB>)
    {
      if (loadable.mStatus == eAsyncStateLoaded)
      {
        TextureLayoutCreate(loadable.mInstance, loadable.mWidth, loadable.mHeight, 0);
        TextureLayoutBind(loadable.mInstance);
        TextureLayoutData(loadable.mInstance, 0, loadable.mpData);
        TextureLayoutUnbind();
        
        loadable.mStatus = eAsyncStateReady;
      }
    }
  }
}