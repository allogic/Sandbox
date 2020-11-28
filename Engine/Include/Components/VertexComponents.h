#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Vertex layouts.
*/

enum VertexLayoutType : u32
{
  eVertexLayoutGizmo,
  eVertexLayoutLambert,
  eVertexLayoutScreen,
};

struct VertexGizmo
{
  constexpr static u32 sType{ eVertexLayoutGizmo };

  r32v3 mPosition{};
  r32v4 mColor   {};
};
struct VertexLambert
{
  constexpr static u32 sType{ eVertexLayoutLambert };

  r32v3 mPosition{};
  r32v3 mNormal  {};
  r32v2 mUv      {};
  r32v4 mColor   {};
};
struct VertexScreen
{
  constexpr static u32 sType{ eVertexLayoutScreen };

  r32v3 mPosition{};
  r32v2 mUv      {};
};

/*
* Mesh components.
*/

template<typename Vertex, typename Index>
struct MeshLayout : Component
{
  using VertexType = Vertex;
  using IndexType  = Index;

  u32              mNumSubMeshes     {};
  std::vector<u32> mVaos             {};
  std::vector<u32> mVbos             {};
  std::vector<u32> mEbos             {};
  std::vector<u32> mVertexBufferSizes{};
  std::vector<u32> mIndexBufferSizes {};
};

using MeshLambert = MeshLayout<VertexLambert, u32>;
using MeshGizmo   = MeshLayout<VertexGizmo, u32>;
using MeshScreen  = MeshLayout<VertexScreen, u32>;

/*
* Mesh management.
*/

template<typename MeshLayout> void MeshLayoutCreate(MeshLayout& meshLayout, u32 numSubMeshes, u32* pVertexBufferSizes, u32* pIndexBufferSizes)
{
  meshLayout.mNumSubMeshes = numSubMeshes;

  meshLayout.mVaos.resize(numSubMeshes);
  meshLayout.mVbos.resize(numSubMeshes);
  meshLayout.mEbos.resize(numSubMeshes);

  meshLayout.mVertexBufferSizes = { pVertexBufferSizes, pVertexBufferSizes + numSubMeshes };
  meshLayout.mIndexBufferSizes = { pIndexBufferSizes, pIndexBufferSizes + numSubMeshes };

  glGenVertexArrays(numSubMeshes, meshLayout.mVaos.data());
  glGenBuffers(numSubMeshes, meshLayout.mVbos.data());
  glGenBuffers(numSubMeshes, meshLayout.mEbos.data());

  for (u32 i{}; i < numSubMeshes; i++)
  {
    glBindVertexArray(meshLayout.mVaos[i]);

    glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[i]);

    switch (MeshLayout::VertexType::sType)
    {
      case eVertexLayoutGizmo:
      {
        glBufferStorage(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSizes[i] * sizeof(MeshLayout::VertexType), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(sizeof(r32v3)));
        break;
      }
      case eVertexLayoutLambert:
      {
        glBufferStorage(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSizes[i] * sizeof(MeshLayout::VertexType), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(sizeof(r32v3)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(sizeof(r32v3) + sizeof(r32v3)));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(sizeof(r32v3) + sizeof(r32v3) + sizeof(r32v2)));
        break;
      }
      case eVertexLayoutScreen:
      {
        glBufferStorage(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSizes[i] * sizeof(MeshLayout::VertexType), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(0));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshLayout::VertexType), (void*)(sizeof(r32v3)));

        break;
      }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[i]);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSizes[i] * sizeof(MeshLayout::IndexType), nullptr, GL_DYNAMIC_STORAGE_BIT);
  }

  glBindVertexArray(0);
}
template<typename MeshLayout> void MeshLayoutBind(MeshLayout const& meshLayout, u32 subMeshIndex)
{
  glBindVertexArray(meshLayout.mVaos[subMeshIndex]);
}
static                        void MeshLayoutUnbind()
{
  glBindVertexArray(0);
}
template<typename MeshLayout> void MeshLayoutData(MeshLayout const& meshLayout, u32 subMeshIndex, void* pVertexData, void* pIndexData)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[subMeshIndex]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, meshLayout.mVertexBufferSizes[subMeshIndex] * sizeof(MeshLayout::VertexType), pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[subMeshIndex]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshLayout.mIndexBufferSizes[subMeshIndex] * sizeof(MeshLayout::IndexType), pIndexData);
}
template<typename MeshLayout> void MeshLayoutDataSub(MeshLayout const& meshLayout, u32 subMeshIndex, void* pVertexData, void* pIndexData, u32 vertexBufferOffset, u32 indexBufferOffset, u32 vertexBufferSizeSub, u32 indexBufferSizeSub)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[subMeshIndex]);
  glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset * sizeof(MeshLayout::VertexType), vertexBufferSizeSub * sizeof(MeshLayout::VertexType), pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[subMeshIndex]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferOffset * sizeof(MeshLayout::IndexType), indexBufferSizeSub * sizeof(MeshLayout::IndexType), pIndexData);
}
template<typename MeshLayout> void MeshLayoutClear(MeshLayout const& meshLayout, u32 subMeshIndex)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[subMeshIndex]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, meshLayout.mVertexBufferSizes[subMeshIndex] * sizeof(MeshLayout::VertexType), nullptr);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[subMeshIndex]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshLayout.mIndexBufferSizes[subMeshIndex] * sizeof(MeshLayout::IndexType), nullptr);
}
template<typename MeshLayout> void MeshLayoutDestroy(MeshLayout const& meshLayout)
{
  glDeleteBuffers(meshLayout.mNumSubMeshes, meshLayout.mVbos.data());
  glDeleteBuffers(meshLayout.mNumSubMeshes, meshLayout.mEbos.data());
  glDeleteVertexArrays(meshLayout.mNumSubMeshes, meshLayout.mVaos.data());
}

/*
* Mesh render methodes.
*/

enum RenderModeType : u32
{
  eRenderModeLine,
  eRenderModeTriangle,
};

template<typename MeshLayout> void MeshLayoutDrawLines(MeshLayout const& meshLayout, u32 subMeshIndex)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_LINES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
template<typename MeshLayout> void MeshLayoutDrawLinesInstanced(MeshLayout const& meshLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_LINES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}

template<typename MeshLayout> void MeshLayoutDrawTriangles(MeshLayout const& meshLayout, u32 subMeshIndex)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_TRIANGLES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
template<typename MeshLayout> void MeshLayoutDrawTrianglesInstanced(MeshLayout const& meshLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_TRIANGLES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}

template<typename MeshLayout> void MeshLayoutRender(MeshLayout const& meshLayout, RenderModeType renderMode)
{
  for (u32 i{}; i < meshLayout.mNumSubMeshes; i++)
  {
    MeshLayoutBind(meshLayout, i);

    switch (renderMode)
    {
      case eRenderModeLine: MeshLayoutDrawLines(meshLayout, i); break;
      case eRenderModeTriangle: MeshLayoutDrawTriangles(meshLayout, i); break;
    }
  }
}
template<typename MeshLayout> void MeshLayoutRenderPartial(MeshLayout const& meshLayout, u32 subMeshIndex, RenderModeType renderMode)
{
  switch (renderMode)
  {
    case eRenderModeLine: MeshLayoutDrawLines(meshLayout, subMeshIndex); break;
    case eRenderModeTriangle: MeshLayoutDrawTriangles(meshLayout, subMeshIndex); break;
  }
}
template<typename MeshLayout> void MeshLayoutRenderInstanced(MeshLayout const& meshLayout, RenderModeType renderMode, u32 numInstances)
{
  for (u32 i{}; i < meshLayout.mNumSubMeshes; i++)
  {
    MeshLayoutBind(meshLayout, i);

    switch (renderMode)
    {
      case eRenderModeLine: MeshLayoutDrawLinesInstanced(meshLayout, i, numInstances); break;
      case eRenderModeTriangle: MeshLayoutDrawTrianglesInstanced(meshLayout, i, numInstances); break;
    }
  }
}
template<typename MeshLayout> void MeshLayoutRenderInstancedPartial(MeshLayout const& meshLayout, u32 subMeshIndex, RenderModeType renderMode, u32 numInstances)
{
  switch (renderMode)
  {
    case eRenderModeLine: MeshLayoutDrawLinesInstanced(meshLayout, subMeshIndex, numInstances); break;
    case eRenderModeTriangle: MeshLayoutDrawTrianglesInstanced(meshLayout, subMeshIndex, numInstances); break;
  }
}