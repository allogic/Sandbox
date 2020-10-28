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

  u32 mVao             {};
  u32 mVertexBufferSize{};
  u32 mIndexBufferSize {};
  u32 mVbo             {};
  u32 mEbo             {};
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

  u32  mNumSubMeshes      {};
  u32  mNumInstances      {};
  u32* mpVaos             {};
  u32* mpVertexBufferSizes{};
  u32* mpIndexBufferSizes {};
  u32* mpVbos             {};
  u32* mpEbos             {};
  u32* mpIbos             {};
};

using ModelLambert = ModelLayout<VertexLambert, u32>;

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

  switch (MeshLayout::VertexType::sType)
  {
    case VertexType::Gizmo:
    {
      glBufferData(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * sizeof(VertexGizmoLine), nullptr, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(0));
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(sizeof(r32v3)));
      break;
    }
    case VertexType::Lambert:
    {
      glBufferData(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * sizeof(VertexLambert), nullptr, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(0));
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3)));
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3)));
      break;
    }
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), nullptr, GL_STATIC_DRAW);

  glBindVertexArray(0);
}
template<typename MeshLayout>                                      void MeshLayoutBind(MeshLayout const& meshLayout)
{
  glBindVertexArray(meshLayout.mVao);
}
template<typename MeshLayout>                                      void MeshLayoutClear(MeshLayout const& meshLayout)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferData(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * sizeof(MeshLayout::VertexType), nullptr, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), nullptr, GL_STATIC_DRAW);
}
template<typename MeshLayout, typename Vertices, typename Indices> void MeshLayoutData(MeshLayout const& meshLayout, Vertices* pVertexData, Indices* pIndexData)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferData(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * sizeof(MeshLayout::VertexType), pVertexData, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSize * sizeof(MeshLayout::IndexType), pIndexData, GL_STATIC_DRAW);
}
template<typename MeshLayout, typename Vertices, typename Indices> void MeshLayoutDataSub(MeshLayout const& meshLayout, Vertices* pVertexData, Indices* pIndexData, u32 vertexBufferOffset, u32 indexBufferOffset, u32 vertexBufferSizeSub, u32 indexBufferSizeSub)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset * sizeof(MeshLayout::VertexType), vertexBufferSizeSub * sizeof(MeshLayout::VertexType), pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferOffset * sizeof(MeshLayout::IndexType), indexBufferSizeSub * sizeof(MeshLayout::IndexType), pIndexData);
}
template<typename MeshLayout>                                      void MeshLayoutRender(MeshLayout const& meshLayout, RenderMode renderMode)
{
  switch (renderMode)
  {
    case Line:
    {
      DrawMeshLayoutLines(meshLayout);
      break;
    }
    case Triangle:
    {
      DrawMeshLayoutTriangles(meshLayout);
      break;
    }
  }
}
template<typename MeshLayout>                                      void MeshLayoutDestroy(MeshLayout const& meshLayout)
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3)));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3)));
        break;
      }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpEbos[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelLayout.mpIndexBufferSizes[i] * sizeof(ModelLayout::IndexType), nullptr, GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}
template<typename ModelLayout>                                      void ModelLayoutCreateInstanced(ModelLayout& modelLayout, u32 numSubMeshes, u32* pVertexBufferSizes, u32* pIndexBufferSizes, u32 numInstances)
{
  modelLayout.mNumSubMeshes = numSubMeshes;
  modelLayout.mNumInstances = numInstances;
  modelLayout.mpVaos = new u32[numSubMeshes];
  modelLayout.mpVertexBufferSizes = new u32[numSubMeshes];
  modelLayout.mpIndexBufferSizes = new u32[numSubMeshes];
  modelLayout.mpVbos = new u32[numSubMeshes];
  modelLayout.mpEbos = new u32[numSubMeshes];
  modelLayout.mpIbos = new u32[numSubMeshes];

  std::memcpy(modelLayout.mpVertexBufferSizes, pVertexBufferSizes, numSubMeshes * sizeof(u32));
  std::memcpy(modelLayout.mpIndexBufferSizes, pIndexBufferSizes, numSubMeshes * sizeof(u32));

  glGenVertexArrays(numSubMeshes, modelLayout.mpVaos);
  glGenBuffers(numSubMeshes, modelLayout.mpVbos);
  glGenBuffers(numSubMeshes, modelLayout.mpEbos);
  glGenBuffers(numSubMeshes, modelLayout.mpIbos);

  for (u32 i{}; i < numSubMeshes; i++)
  {
    glBindVertexArray(modelLayout.mpVaos[i]);

    glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpVbos[i]);
    glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[i] * sizeof(VertexGizmoLine), nullptr, GL_STATIC_DRAW);

    switch (ModelLayout::VertexType::sType)
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3)));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLambert), (void*)(sizeof(r32v3) + sizeof(r32v3)));
        break;
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpIbos[i]);
    glBufferData(GL_ARRAY_BUFFER, modelLayout.mNumInstances * sizeof(r32m4), nullptr, GL_STATIC_DRAW);

    switch (ModelLayout::VertexType::sType)
    {
      case VertexType::Gizmo:
      {
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(0));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4)));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4) + sizeof(r32m4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4) + sizeof(r32m4) + sizeof(r32m4)));
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        break;
      }
      case VertexType::Lambert:
      {
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(0));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4) + sizeof(r32m4)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(r32m4), (void*)(sizeof(r32m4) + sizeof(r32m4) + sizeof(r32m4)));
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
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
template<typename ModelLayout, typename Vertices, typename Indices> void ModelLayoutDataInstanced(ModelLayout const& modelLayout, u32 subMeshIndex, Vertices* pVertexData, Indices* pIndexData, r32m4* pProjectionData)
{
  glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpVbos[subMeshIndex]);
  glBufferData(GL_ARRAY_BUFFER, modelLayout.mpVertexBufferSizes[subMeshIndex] * sizeof(ModelLayout::VertexType), pVertexData, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, modelLayout.mpIbos[subMeshIndex]);
  glBufferData(GL_ARRAY_BUFFER, modelLayout.mNumInstances * sizeof(r32m4), pProjectionData, GL_STATIC_DRAW);

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
template<typename ModelLayout>                                      void ModelLayoutRenderInstanced(ModelLayout const& modelLayout, u32 subMeshIndex, RenderMode renderMode)
{
  switch (renderMode)
  {
    case Line:
    {
      DrawModelLayoutLinesInstanced(modelLayout, subMeshIndex, modelLayout.mNumInstances);
      break;
    }
    case Triangle:
    {
      DrawModelLayoutTrianglesInstanced(modelLayout, subMeshIndex, modelLayout.mNumInstances);
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
template<typename ModelLayout>                                      void ModelLayoutDestroyInstanced(ModelLayout const& modelLayout)
{
  glDeleteBuffers(modelLayout.mNumSubMeshes, modelLayout.mpVbos);
  glDeleteBuffers(modelLayout.mNumSubMeshes, modelLayout.mpEbos);
  glDeleteBuffers(modelLayout.mNumSubMeshes, modelLayout.mpIbos);
  glDeleteVertexArrays(modelLayout.mNumSubMeshes, modelLayout.mpVaos);

  delete[] modelLayout.mpVaos;
  delete[] modelLayout.mpVertexBufferSizes;
  delete[] modelLayout.mpIndexBufferSizes;
  delete[] modelLayout.mpVbos;
  delete[] modelLayout.mpEbos;
  delete[] modelLayout.mpIbos;
}