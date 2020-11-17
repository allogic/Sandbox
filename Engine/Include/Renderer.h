#pragma once

#include <Core.h>
#include <Types.h>
#include <Registry.h>
#include <Camera.h>
#include <FrameBuffer.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>
#include <Layouts/TextureLayouts.h>

#include <Materials/RenderMaterials.h>
#include <Materials/ScreenMaterials.h>

/*
* Render tasks.
*/

struct TaskLambert
{
  ModelLambert*   mpModel        {};
  TextureR32RGBA* mpTextureAlbedo{};
};
struct TaskLambertInstanced
{
  ModelLambert*            mpModel          {};
  BufferLayout<Transform>* mpBufferTransform{};
  TextureR32RGBA*          mpTextureAlbedo  {};
  u32                      mNumInstances    {};
};

/*
* Renderer.
*/

struct Renderer
{
  u32                                   mWidth                      {};
  u32                                   mHeight                     {};

  Camera&                               mCamera                     { RegistryGetOrCreate<Camera>("camera") };

  UniformLayout<UniformBlockProjection> mUniformProjection          {};
  UniformLayout<UniformBlockScreen>     mUniformScreen              {};
  UniformLayout<UniformBlockPointLight> mUniformPointLight          {};

  RenderMaterialLambert                 mMaterialLambert            {};
  RenderMaterialLambertInstanced        mMaterialLambertInstanced   {};
  RenderMaterialGizmo                   mMaterialGizmo              {}; 

  ScreenMaterialDefault                 mMaterialDeferredLight      {};

  std::queue<TaskLambert>               mRenderQueueLambert         {};
  std::queue<TaskLambertInstanced>      mRenderQueueLambertInstanced{};

  FrameBuffer                           mFrameBufferDeferred        {};

  TextureR32RGBA                        mTexturePosition            {};
  TextureR32RGBA                        mTextureAlbedo              {};
  TextureR32RGBA                        mTextureNormal              {};
  TextureR32RGBA                        mTextureUv                  {};

  DepthR32RGBA                          mDepthBuffer                {};

  MeshScreen                            mMeshScreenPlane            {};
  MeshGizmo                             mMeshGizmoLineBatch         {};

  u32                                   mVertexOffsetGizmoLineBatch {};
  u32                                   mIndexOffsetGizmoLineBatch  {};
};

/*
* Renderer management.
*/

template<typename Renderer> void RendererCreate(Renderer& renderer, u32 width, u32 height)
{
  renderer.mWidth = width;
  renderer.mHeight = height;

  UniformLayoutCreate(renderer.mUniformProjection);
  UniformLayoutCreate(renderer.mUniformScreen);
  UniformLayoutCreate(renderer.mUniformPointLight);

  RenderMaterialCreate(renderer.mMaterialLambert);
  RenderMaterialCreate(renderer.mMaterialLambertInstanced);
  RenderMaterialCreate(renderer.mMaterialGizmo);

  ScreenMaterialCreate(renderer.mMaterialDeferredLight, "DeferredLightPass");

  FrameBufferCreate(renderer.mFrameBufferDeferred, renderer.mWidth, renderer.mHeight);

  TextureLayoutCreate(renderer.mTexturePosition, renderer.mWidth, renderer.mHeight);
  TextureLayoutCreate(renderer.mTextureAlbedo, renderer.mWidth, renderer.mHeight);
  TextureLayoutCreate(renderer.mTextureNormal, renderer.mWidth, renderer.mHeight);
  TextureLayoutCreate(renderer.mTextureUv, renderer.mWidth, renderer.mHeight);
  TextureLayoutCreate(renderer.mDepthBuffer, renderer.mWidth, renderer.mHeight);

  MeshLayoutCreate(renderer.mMeshScreenPlane, 4, 6);
  MeshLayoutBind(renderer.mMeshScreenPlane);
  std::vector<VertexScreen> vertices
  {
    { { -1.f, -1.f, 0.f }, { 0.f, 0.f } },
    { {  1.f, -1.f, 0.f }, { 1.f, 0.f } },
    { { -1.f,  1.f, 0.f }, { 0.f, 1.f } },
    { {  1.f,  1.f, 0.f }, { 1.f, 1.f } },
  };
  std::vector<u32> indices
  {
    0, 1, 2, 2, 3, 1,
  };
  MeshLayoutData(renderer.mMeshScreenPlane, vertices.data(), indices.data());

  MeshLayoutCreate(renderer.mMeshGizmoLineBatch, 65535, 65535 * 2);

  FrameBufferBindWrite(renderer.mFrameBufferDeferred);

  FrameBufferAttachTexture(renderer.mTexturePosition, 0);
  FrameBufferAttachTexture(renderer.mTextureAlbedo, 1);
  FrameBufferAttachTexture(renderer.mTextureNormal, 2);
  FrameBufferAttachTexture(renderer.mTextureUv, 3);

  FrameBufferAttachDepthBuffer(renderer.mDepthBuffer);

  u32 buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

  glDrawBuffers(4, buffers);

  FrameBufferUnbindWrite();
}
template<typename Renderer> void RendererSubmitLambert(Renderer& renderer, TaskLambert const& taskLambert)
{
  renderer.mRenderQueueLambert.emplace(taskLambert);
}
template<typename Renderer> void RendererSubmitLambertInstanced(Renderer& renderer, TaskLambertInstanced const& taskLambertInstanced)
{
  renderer.mRenderQueueLambertInstanced.emplace(taskLambertInstanced);
}
template<typename Renderer> void RendererDispatchLambert(Renderer& renderer)
{
  UniformBlockProjection projection{};
  projection.mCameraPosition = renderer.mCamera.mPosition;
  projection.mProjection = renderer.mCamera.mProjection;
  projection.mView = renderer.mCamera.mView;

  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialLambert);

  while (!renderer.mRenderQueueLambert.empty())
  {
    TaskLambert const& taskLambert{ renderer.mRenderQueueLambert.front() };

    projection.mTransform = taskLambert.mpModel->mTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &projection);

    TextureLayoutBind(*taskLambert.mpTextureAlbedo);
    TextureLayoutMapSampler(*taskLambert.mpTextureAlbedo, 0);

    for (u32 i{}; i < taskLambert.mpModel->mNumSubMeshes; i++)
    {
      ModelLayoutBind(*taskLambert.mpModel, i);
      ModelLayoutRender(*taskLambert.mpModel, i, eRenderModeTriangle);
    }

    renderer.mRenderQueueLambert.pop();
  }

  TextureLayoutUnbind();
}
template<typename Renderer> void RendererDispatchLambertInstanced(Renderer& renderer)
{
  UniformBlockProjection projection{};
  projection.mCameraPosition = renderer.mCamera.mPosition;
  projection.mProjection = renderer.mCamera.mProjection;
  projection.mView = renderer.mCamera.mView;

  while (!renderer.mRenderQueueLambertInstanced.empty())
  {
    TaskLambertInstanced const& taskLambertInstanced{ renderer.mRenderQueueLambertInstanced.front() };

    BufferLayoutBind(*taskLambertInstanced.mpBufferTransform);
    BufferLayoutMap(*taskLambertInstanced.mpBufferTransform, 0);

    UniformLayoutBind(renderer.mUniformProjection);
    UniformLayoutMap(renderer.mUniformProjection, 0);

    TextureLayoutBind(*taskLambertInstanced.mpTextureAlbedo);
    TextureLayoutMapSampler(*taskLambertInstanced.mpTextureAlbedo, 0);

    RenderMaterialBind(renderer.mMaterialLambertInstanced);

    for (u32 i{}; i < taskLambertInstanced.mpModel->mNumSubMeshes; i++)
    {
      ModelLayoutBind(*taskLambertInstanced.mpModel, i);
      ModelLayoutRenderInstanced(*taskLambertInstanced.mpModel, i, eRenderModeTriangle, taskLambertInstanced.mNumInstances);
    }

    renderer.mRenderQueueLambertInstanced.pop();
  }

  TextureLayoutUnbind();
}
template<typename Renderer> void RendererDispatchGizmo(Renderer& renderer)
{
  UniformBlockProjection projection{};
  projection.mProjection = renderer.mCamera.mProjection;
  projection.mView = renderer.mCamera.mView;
  projection.mTransform = glm::identity<r32m4>();

  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutDataSet(renderer.mUniformProjection, 1, &projection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialGizmo);
    
  MeshLayoutBind(renderer.mMeshGizmoLineBatch);
  MeshLayoutRender(renderer.mMeshGizmoLineBatch, eRenderModeLine);

  renderer.mVertexOffsetGizmoLineBatch = 0;
  renderer.mIndexOffsetGizmoLineBatch = 0;
}
template<typename Renderer> void RendererRenderDeferred(Renderer& renderer)
{
  SANDBOX_ENGINE_MEASURE_BEGIN(RendererDeferredPassGeometry);
  
  FrameBufferBindWrite(renderer.mFrameBufferDeferred);

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  RendererDispatchLambert(renderer);
  RendererDispatchLambertInstanced(renderer);
  glDisable(GL_BLEND);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  FrameBufferUnbind();

  SANDBOX_ENGINE_MEASURE_END(RendererDeferredPassGeometry);

  SANDBOX_ENGINE_MEASURE_BEGIN(RendererDeferredPassLight);

  FrameBufferBindRead(renderer.mFrameBufferDeferred);

  //FrameBufferSetReadTexture(0);
  //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  TextureLayoutBind(renderer.mTexturePosition);
  TextureLayoutMapSampler(renderer.mTexturePosition, 0);
  
  TextureLayoutBind(renderer.mTextureAlbedo);
  TextureLayoutMapSampler(renderer.mTextureAlbedo, 1);
  
  TextureLayoutBind(renderer.mTextureNormal);
  TextureLayoutMapSampler(renderer.mTextureNormal, 2);
  
  TextureLayoutBind(renderer.mTextureUv);
  TextureLayoutMapSampler(renderer.mTextureUv, 3);
  
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  UniformLayoutBind(renderer.mUniformPointLight);
  UniformLayoutMap(renderer.mUniformPointLight, 1);

  ScreenMaterialBind(renderer.mMaterialDeferredLight);
  
  MeshLayoutBind(renderer.mMeshScreenPlane);
  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  FrameBufferUnbind();

  SANDBOX_ENGINE_MEASURE_END(RendererDeferredPassLight);
}
template<typename Renderer> void RendererRenderGizmo(Renderer& renderer)
{
  SANDBOX_ENGINE_MEASURE_BEGIN(RendererGizmoPass);

  RendererDispatchGizmo(renderer);

  SANDBOX_ENGINE_MEASURE_END(RendererGizmoPass);
}
template<typename Renderer> void RendererDestroy(Renderer const& renderer)
{
  UniformLayoutDestroy(renderer.mUniformProjection);
  UniformLayoutDestroy(renderer.mUniformScreen);
  UniformLayoutDestroy(renderer.mUniformPointLight);

  RenderMaterialDestroy(renderer.mMaterialLambert);
  RenderMaterialDestroy(renderer.mMaterialLambertInstanced);
  RenderMaterialDestroy(renderer.mMaterialGizmo);

  ScreenMaterialDestroy(renderer.mMaterialDeferredLight);

  FrameBufferDestroy(renderer.mFrameBufferDeferred);

  TextureLayoutDestroy(renderer.mTexturePosition);
  TextureLayoutDestroy(renderer.mTextureAlbedo);
  TextureLayoutDestroy(renderer.mTextureNormal);
  TextureLayoutDestroy(renderer.mTextureUv);
  TextureLayoutDestroy(renderer.mDepthBuffer);

  MeshLayoutDestroy(renderer.mMeshScreenPlane);
  MeshLayoutDestroy(renderer.mMeshGizmoLineBatch);
}

/*
* Renderer debug utilities.
*/

template<typename Renderer> void RendererLineBatchPushLine(Renderer& renderer, r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  VertexGizmo vertices[2]{ { p0, color }, { p1, color } };
  u32 indices[2]{ renderer.mVertexOffsetGizmoLineBatch + 0, renderer.mIndexOffsetGizmoLineBatch + 1 };

  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 2, 2);

  renderer.mVertexOffsetGizmoLineBatch += 2;
  renderer.mIndexOffsetGizmoLineBatch += 2;
}
template<typename Renderer> void RendererLineBatchPushBox(Renderer const& renderer, r32v3 const& position, r32v3 const& size, r32v4 const& color)
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
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 1,
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 1,

    // back
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 5,
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 7,
    renderer.mVertexOffsetGizmoLineBatch + 7,
    renderer.mVertexOffsetGizmoLineBatch + 5,

    // connections
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 1,
    renderer.mVertexOffsetGizmoLineBatch + 5,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 7,
  };

  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 8, 24);

  renderer.mVertexOffsetGizmoLineBatch += 8;
  renderer.mIndexOffsetGizmoLineBatch += 24;
}