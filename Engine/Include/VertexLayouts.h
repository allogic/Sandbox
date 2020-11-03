#pragma once

#include <Core.h>
#include <Types.h>

/*
* Vertex layouts.
*/

enum VertexType : u32
{
  Gizmo,
  Lambert,
};

struct VertexGizmoLine
{
  constexpr static VertexType sType{ Gizmo };

  r32v3 mPosition{};
  r32v4 mColor   {};
};
struct VertexLambert
{
  constexpr static VertexType sType{ Lambert };

  r32v3 mPosition{};
  r32v3 mNormal  {};
  r32v2 mUv      {};
  r32v4 mColor   {};
};

/*
* Mesh layouts.
*/

template<typename Vertex, typename Index>
struct MeshLayout
{
  using VertexType = Vertex;
  using IndexType  = Index;

  u32   mVao             {};
  u32   mVertexBufferSize{};
  u32   mIndexBufferSize {};
  u32   mVbo             {};
  u32   mEbo             {};
  r32m4 mTransform       { glm::identity<r32m4>() };
};

using MeshGizmo = MeshLayout<VertexGizmoLine, u32>;

/*
* Model layouts.
*/

template<typename Vertex, typename Index>
struct ModelLayout
{
  using VertexType = Vertex;
  using IndexType  = Index;

  u32   mNumSubMeshes      {};
  u32*  mpVaos             {};
  u32*  mpVertexBufferSizes{};
  u32*  mpIndexBufferSizes {};
  u32*  mpVbos             {};
  u32*  mpEbos             {};
  r32m4 mTransform         { glm::identity<r32m4>() };
};

using ModelGizmoLine = ModelLayout<VertexGizmoLine, u32>;
using ModelLambert   = ModelLayout<VertexLambert, u32>;

/*
* Drawing methodes.
*/

enum RenderMode : u32
{
  Line,
  Triangle,
};

template<typename MeshLayout>  void DrawMeshLayoutLines(MeshLayout const& meshLayout)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_LINES, (s32)meshLayout.mIndexBufferSize, GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
template<typename MeshLayout>  void DrawMeshLayoutTriangles(MeshLayout const& meshLayout)
{
  switch (sizeof(MeshLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_TRIANGLES, (s32)meshLayout.mIndexBufferSize, GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}

template<typename ModelLayout> void DrawModelLayoutLines(ModelLayout const& modelLayout, u32 subMeshIndex)
{
  switch (sizeof(ModelLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_LINES, (s32)modelLayout.mpIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
template<typename ModelLayout> void DrawModelLayoutLinesInstanced(ModelLayout const& modelLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(ModelLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_LINES, (s32)modelLayout.mpIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}
template<typename ModelLayout> void DrawModelLayoutTriangles(ModelLayout const& modelLayout, u32 subMeshIndex)
{
  switch (sizeof(ModelLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_TRIANGLES, (s32)modelLayout.mpIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
template<typename ModelLayout> void DrawModelLayoutTrianglesInstanced(ModelLayout const& modelLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(ModelLayout::IndexType))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_TRIANGLES, (s32)modelLayout.mpIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}

/*
* VAO management.
*/

template<typename MeshLayout>                                      void MeshLayoutCreate(MeshLayout& meshLayout, u32 vertexBufferSize, u32 indexBufferSize)
{
  glGenVertexArrays(1, &meshLayout.mVao);

  meshLayout.mVertexBufferSize = vertexBufferSize;
  meshLayout.mIndexBufferSize = indexBufferSize;

  glGenBuffers(1, &meshLayout.mVbo);
  glGenBuffers(1, &meshLayout.mEbo);

  glBindVertexArray(meshLayout.mVao);

  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferStorage(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * sizeof(VertexGizmoLine), nullptr, GL_DYNAMIC_STORAGE_BIT);

  switch (MeshLayout::VertexType::sType)
  {
    case VertexType::Gizmo:
    {
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(0));
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(sizeof(r32v3)));
      break;
    }
    case VertexType::Lambert:
    {
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(0));
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3)));
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3)));
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3) + sizeof(r32v2)));
      break;
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), nullptr, GL_DYNAMIC_STORAGE_BIT);

  glBindVertexArray(0);
}
template<typename MeshLayout> void MeshLayoutBind(MeshLayout const& meshLayout)
{
  glBindVertexArray(meshLayout.mVao);
}
template<typename MeshLayout> void MeshLayoutUnbind(MeshLayout const& meshLayout)
{
  glBindVertexArray(0);
}
template<typename MeshLayout> void MeshLayoutTransform(MeshLayout& meshLayout, r32v3 position, r32v3 rotation, r32v3 scale)
{
  meshLayout.mTransform = glm::translate(meshLayout.mTransform, position);

  meshLayout.mTransform = glm::rotate(meshLayout.mTransform, rotation.x, { 1.f, 0.f, 0.f });
  meshLayout.mTransform = glm::rotate(meshLayout.mTransform, rotation.y, { 0.f, 1.f, 0.f });
  meshLayout.mTransform = glm::rotate(meshLayout.mTransform, rotation.z, { 0.f, 0.f, 1.f });

  meshLayout.mTransform = glm::scale(meshLayout.mTransform, scale);
}
template<typename MeshLayout> void MeshLayoutClear(MeshLayout const& meshLayout)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, meshLayout.mVertexBufferSize * sizeof(MeshLayout::VertexType), nullptr);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), nullptr);
}
template<typename MeshLayout> void MeshLayoutData(MeshLayout const& meshLayout, void* pVertexData, void* pIndexData)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, meshLayout.mVertexBufferSize * sizeof(MeshLayout::VertexType), pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), pIndexData);
}
template<typename MeshLayout> void MeshLayoutDataSub(MeshLayout const& meshLayout, void* pVertexData, void* pIndexData, u32 vertexBufferOffset, u32 indexBufferOffset, u32 vertexBufferSizeSub, u32 indexBufferSizeSub)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset * sizeof(MeshLayout::VertexType), vertexBufferSizeSub * sizeof(MeshLayout::VertexType), pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferOffset * sizeof(MeshLayout::IndexType), indexBufferSizeSub * sizeof(MeshLayout::IndexType), pIndexData);
}
template<typename MeshLayout> void MeshLayoutRender(MeshLayout const& meshLayout, RenderMode renderMode)
{
  switch (renderMode)
  {
    case Line: DrawMeshLayoutLines(meshLayout); break;
    case Triangle: DrawMeshLayoutTriangles(meshLayout); break;
  }
}
template<typename MeshLayout> void MeshLayoutDestroy(MeshLayout const& meshLayout)
{
  glDeleteBuffers(1, &meshLayout.mVbo);
  glDeleteBuffers(1, &meshLayout.mEbo);
  glDeleteVertexArrays(1, &meshLayout.mVao);
}

/*
* VAO's management.
*/

template<typename ModelLayout>                                      void ModelLayoutCreate(ModelLayout& modelLayout, u32 numSubMeshes, u32* pVertexBufferSizes, u32* pIndexBufferSizes)
{
  modelLayout.mpVaos = new u32[numSubMeshes];
  modelLayout.mNumSubMeshes = numSubMeshes;
  modelLayout.mpVertexBufferSizes = new u32[numSubMeshes];
  modelLayout.mpIndexBufferSizes = new u32[numSubMeshes];
  modelLayout.mpVbos = new u32[numSubMeshes];
  modelLayout.mpEbos = new u32[numSubMeshes];

  std::memcpy(modelLayout.mpVertexBufferSizes, pVertexBufferSizes, numSubMeshes * sizeof(u32));
  std::memcpy(modelLayout.mpIndexBufferSizes, pIndexBufferSizes, numSubMeshes * sizeof(u32));

  glGenVertexArrays(numSubMeshes, modelLayout.mpVaos);
  glGenBuffers(numSubMeshes, modelLayout.mpVbos);
  glGenBuffers(numSubMeshes, modelLayout.mpEbos);

  for (u32 i{}; i < numSubMeshes; i++)
  {
    glBindVertexArray(modelLayout.mpVaos[i]);

    glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpVbos[i]);

    switch (ModelLayout::VertexType::sType)
    {
      case VertexType::Gizmo:
      {
        glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[i] * sizeof(VertexGizmoLine), nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(sizeof(r32v3)));
        break;
      }
      case VertexType::Lambert:
      {
        glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[i] * sizeof(VertexLambert), nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3)));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3) + sizeof(r32v2)));
        break;
      }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpEbos[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpIndexBufferSizes[i] * sizeof(ModelLayout::IndexType), nullptr, GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}
template<typename ModelLayout>                                      void ModelLayoutBind(ModelLayout const& modelLayout, u32 subMeshIndex)
{
  glBindVertexArray(modelLayout.mpVaos[subMeshIndex]);
}
template<typename ModelLayout>                                      void ModelLayoutTransform(ModelLayout& modelLayout, r32v3 position, r32v3 rotation, r32v3 scale)
{
  modelLayout.mTransform = glm::translate(modelLayout.mTransform, position);

  modelLayout.mTransform = glm::rotate(modelLayout.mTransform, rotation.x, { 1.f, 0.f, 0.f });
  modelLayout.mTransform = glm::rotate(modelLayout.mTransform, rotation.y, { 0.f, 1.f, 0.f });
  modelLayout.mTransform = glm::rotate(modelLayout.mTransform, rotation.z, { 0.f, 0.f, 1.f });

  modelLayout.mTransform = glm::scale(modelLayout.mTransform, scale);
}
template<typename ModelLayout>                                      void ModelLayoutClear(ModelLayout const& modelLayout, u32 subMeshIndex)
{
  glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpVbos[subMeshIndex]);
  glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[subMeshIndex] * sizeof(ModelLayout::VertexType), nullptr, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpEbos[subMeshIndex]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpIndexBufferSizes[subMeshIndex] * sizeof(ModelLayout::IndexType), nullptr, GL_STATIC_DRAW);
}
template<typename ModelLayout, typename Vertices, typename Indices> void ModelLayoutData(ModelLayout const& modelLayout, u32 subMeshIndex, Vertices* pVertexData, Indices* pIndexData)
{
  glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpVbos[subMeshIndex]);
  glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[subMeshIndex] * sizeof(ModelLayout::VertexType), pVertexData, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpEbos[subMeshIndex]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpIndexBufferSizes[subMeshIndex] * sizeof(ModelLayout::IndexType), pIndexData, GL_STATIC_DRAW);
}
template<typename ModelLayout>                                      void ModelLayoutRender(ModelLayout const& modelLayout, u32 subMeshIndex, RenderMode renderMode)
{
  switch (renderMode)
  {
    case Line:
    {
      DrawModelLayoutLines(modelLayout, subMeshIndex);
      break;
    }
    case Triangle:
    {
      DrawModelLayoutTriangles(modelLayout, subMeshIndex);
      break;
    }
  }
}
template<typename ModelLayout>                                      void ModelLayoutRenderInstanced(ModelLayout const& modelLayout, u32 subMeshIndex, RenderMode renderMode, u32 numInstances)
{
  switch (renderMode)
  {
    case Line:
    {
      DrawModelLayoutLinesInstanced(modelLayout, subMeshIndex, numInstances);
      break;
    }
    case Triangle:
    {
      DrawModelLayoutTrianglesInstanced(modelLayout, subMeshIndex, numInstances);
      break;
    }
  }
}
template<typename ModelLayout>                                      void ModelLayoutDestroy(ModelLayout const& modelLayout)
{
  glDeleteBuffers(modelLayout.mNumSubMeshes, modelLayout.mpVbos);
  glDeleteBuffers(modelLayout.mNumSubMeshes, modelLayout.mpEbos);
  glDeleteVertexArrays(modelLayout.mNumSubMeshes, modelLayout.mpVaos);

  delete[] modelLayout.mpVaos;
  delete[] modelLayout.mpVertexBufferSizes;
  delete[] modelLayout.mpIndexBufferSizes;
  delete[] modelLayout.mpVbos;
  delete[] modelLayout.mpEbos;
}