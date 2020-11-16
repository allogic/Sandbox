#include <Renderer.h>

/*
void Renderer::LineBatchCreate()
{
  MeshLayoutCreate(mMeshGizmoLineBatch, mVertexBufferSizeGizmoLineBatch, mVertexBufferSizeGizmoLineBatch * 2);
}
void Renderer::LineBatchClear()
{
  MeshLayoutBind(mMeshGizmoLineBatch);
  MeshLayoutClear(mMeshGizmoLineBatch);

  mVertexOffsetGizmoLineBatch = 0;
  mIndexOffsetGizmoLineBatch = 0;
}
void Renderer::LineBatchBind()
{
  MeshLayoutBind(mMeshGizmoLineBatch);
}
void Renderer::LineBatchPushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  VertexGizmo vertices[2]{ { p0, color }, { p1, color } };
  u32 indices[2]{ mVertexOffsetGizmoLineBatch + 0, mVertexOffsetGizmoLineBatch + 1 };

  MeshLayoutDataSub(mMeshGizmoLineBatch, vertices, indices, mVertexOffsetGizmoLineBatch, mIndexOffsetGizmoLineBatch, 2, 2);

  mVertexOffsetGizmoLineBatch += 2;
  mIndexOffsetGizmoLineBatch += 2;
}
void Renderer::LineBatchPushBox(r32v3 const& position, r32v3 const& size, r32v4 const& color)
{
  r32v3 half{ size / 2.f };

  r32v3 blf{ -half.x, -half.y, -half.z };
  r32v3 brf{ half.x, -half.y, -half.z };
  r32v3 tlf{ -half.x,  half.y, -half.z };
  r32v3 trf{ half.x,  half.y, -half.z };

  r32v3 blb{ -half.x, -half.y,  half.z };
  r32v3 brb{ half.x, -half.y,  half.z };
  r32v3 tlb{ -half.x,  half.y,  half.z };
  r32v3 trb{ half.x,  half.y,  half.z };

  VertexGizmo vertices[8]
  {
    // front
    { position + blf, color },
    { position + brf, color },
    { position + tlf, color },
    { position + trf, color },

    // back
    { position + blb, color },
    { position + brb, color },
    { position + tlb, color },
    { position + trb, color },
  };
  u32 indices[24]
  {
    // front
    mVertexOffsetGizmoLineBatch + 0,
    mVertexOffsetGizmoLineBatch + 1,
    mVertexOffsetGizmoLineBatch + 0,
    mVertexOffsetGizmoLineBatch + 2,
    mVertexOffsetGizmoLineBatch + 2,
    mVertexOffsetGizmoLineBatch + 3,
    mVertexOffsetGizmoLineBatch + 3,
    mVertexOffsetGizmoLineBatch + 1,

    // back
    mVertexOffsetGizmoLineBatch + 4,
    mVertexOffsetGizmoLineBatch + 5,
    mVertexOffsetGizmoLineBatch + 4,
    mVertexOffsetGizmoLineBatch + 6,
    mVertexOffsetGizmoLineBatch + 6,
    mVertexOffsetGizmoLineBatch + 7,
    mVertexOffsetGizmoLineBatch + 7,
    mVertexOffsetGizmoLineBatch + 5,

    // connections
    mVertexOffsetGizmoLineBatch + 0,
    mVertexOffsetGizmoLineBatch + 4,
    mVertexOffsetGizmoLineBatch + 1,
    mVertexOffsetGizmoLineBatch + 5,
    mVertexOffsetGizmoLineBatch + 2,
    mVertexOffsetGizmoLineBatch + 6,
    mVertexOffsetGizmoLineBatch + 3,
    mVertexOffsetGizmoLineBatch + 7,
  };

  MeshLayoutDataSub(mMeshGizmoLineBatch, vertices, indices, mVertexOffsetGizmoLineBatch, mIndexOffsetGizmoLineBatch, 8, 24);

  mVertexOffsetGizmoLineBatch += 8;
  mIndexOffsetGizmoLineBatch += 24;
}
void Renderer::LineBatchRender()
{
  RenderMaterialBind(mMaterialGizmo);
  MeshLayoutRender(mMeshGizmoLineBatch, eRenderModeLine);

  mVertexOffsetGizmoLineBatch = 0;
  mIndexOffsetGizmoLineBatch = 0;
}
*/