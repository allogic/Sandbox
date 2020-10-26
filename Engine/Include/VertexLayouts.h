#pragma once

#include <Core.h>
#include <Types.h>

/*
* Vertex layouts.
*/

struct VertexGizmoLine
{
  r32v3 mPosition{};
  r32v4 mColor   {};
};
struct VertexLambert
{
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

  constexpr static u32 sVertexSizeBytes{ sizeof(Vertex) };
  constexpr static u32 sIndexSizeBytes { sizeof(Index) };

  u32 mVertexBufferSize{};
  u32 mIndexBufferSize {};
  u32 mVao             {};
  u32 mVbo             {};
  u32 mEbo             {};
};

using MeshGizmo   = MeshLayout<VertexGizmoLine, u32>;
using MeshLambert = MeshLayout<VertexLambert, u32>;

/*
* Model layouts.
*/

template<typename MeshLayout>
struct ModelLayout
{
  // further abstract into array of VAO's
  std::vector<MeshLayout> mMeshes;
};

using ModelLambert = ModelLayout<MeshLambert>;

/*
* VAO drawing methods.
*/

enum RenderMode : u32
{
  Lines,
  Triangles,
};

template<typename MeshLayout> void DrawLayoutLines(MeshLayout const& meshLayout)
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
template<typename MeshLayout> void DrawLayoutTriangles(MeshLayout const& meshLayout)
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

/*
* VAO management.
*/

template<typename MeshLayout>                                      void MeshLayoutCreate(MeshLayout& meshLayout, u32 vertexBufferSize, u32 indexBufferSize)
{
  meshLayout.mVertexBufferSize = vertexBufferSize;
  meshLayout.mIndexBufferSize = indexBufferSize;

  glGenVertexArrays(1, &meshLayout.mVao);
  glGenBuffers(1, &meshLayout.mVbo);
  glGenBuffers(1, &meshLayout.mEbo);

  glBindVertexArray(meshLayout.mVao);

  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);

  switch (sizeof(MeshLayout::VertexType))
  {
    case sizeof(VertexGizmoLine):
    {
      glBufferData(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(VertexGizmoLine), nullptr, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(0));
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGizmoLine), (void*)(sizeof(r32v3)));
      break;
    }
    case sizeof(VertexLambert):
    {
      glBufferData(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(VertexLambert), nullptr, GL_STATIC_DRAW);
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize * sizeof(MeshLayout::IndexType), nullptr, GL_STATIC_DRAW);

  glBindVertexArray(0);
}
template<typename MeshLayout>                                      void MeshLayoutBind(MeshLayout const& meshLayout)
{
  glBindVertexArray(meshLayout.mVao);
}
template<typename MeshLayout>                                      void MeshLayoutUnbind(MeshLayout const& meshLayout)
{
  glBindVertexArray(0);
}
template<typename MeshLayout>                                      void MeshLayoutClear(MeshLayout const& meshLayout)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferData(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSize * MeshLayout::sVertexSizeBytes, nullptr, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSize * MeshLayout::sIndexSizeBytes, nullptr, GL_STATIC_DRAW);
}
template<typename MeshLayout, typename Vertices, typename Indices> void MeshLayoutData(MeshLayout const& meshLayout, Vertices* pVertexData, Indices* pIndexData, u32 vertexBufferSize, u32 indexBufferSize)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize * MeshLayout::sVertexSizeBytes, pVertexData, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize * MeshLayout::sIndexSizeBytes, pIndexData, GL_STATIC_DRAW);
}
template<typename MeshLayout, typename Vertices, typename Indices> void MeshLayoutDataSub(MeshLayout const& meshLayout, Vertices* pVertexData, Indices* pIndexData, u32 vertexBufferOffset, u32 indexBufferOffset, u32 vertexBufferSize, u32 indexBufferSize)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset * MeshLayout::sVertexSizeBytes, vertexBufferSize * MeshLayout::sVertexSizeBytes, pVertexData);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferOffset * MeshLayout::sIndexSizeBytes, indexBufferSize * MeshLayout::sIndexSizeBytes, pIndexData);
}
template<typename MeshLayout>                                      void MeshLayoutRender(MeshLayout const& meshLayout, RenderMode renderMode)
{
  switch (renderMode)
  {
    case Lines:
    {
      DrawLayoutLines(meshLayout);
      break;
    }
    case Triangles:
    {
      DrawLayoutTriangles(meshLayout);
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