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

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>

#include <Buffers/Transform.h>

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

  r32 const&                            mTime                       { RegistryGetOrCreate<r32>("time") };
  r32 const&                            mTimeDelta                  { RegistryGetOrCreate<r32>("timeDelta") };
  Camera const&                         mCamera                     { RegistryGetOrCreate<Camera>("camera") };

  UniformBlockGlobal                    mUniformBlockGlobal         {};
  UniformBlockProjection                mUniformBlockProjection     {};
  UniformBlockCamera                    mUniformBlockCamera         {};
  UniformBlockPointLight                mUniformBlockPointLights[32]{};

  UniformLayout<UniformBlockGlobal>     mUniformGlobal              {};
  UniformLayout<UniformBlockProjection> mUniformProjection          {};
  UniformLayout<UniformBlockCamera>     mUniformCamera              {};
  UniformLayout<UniformBlockPointLight> mUniformPointLights         {};

  RenderMaterialLambert                 mMaterialLambert            {};
  RenderMaterialLambertInstanced        mMaterialLambertInstanced   {};
  RenderMaterialGizmo                   mMaterialGizmo              {}; 

  ScreenMaterialDefault                 mMaterialDeferredLight      {};
  ScreenMaterialDefault                 mMaterialVolumeCloud        {};

  std::queue<TaskLambert>               mRenderQueueLambert         {};
  std::queue<TaskLambertInstanced>      mRenderQueueLambertInstanced{};

  FrameBuffer                           mFrameBufferDeferred        {};

  TextureR32RGBA                        mTexturePosition            {};
  TextureR32RGBA                        mTextureAlbedo              {};
  TextureR32RGBA                        mTextureNormal              {};
  TextureR32RGBA                        mTextureUv                  {};

  DepthR32RGBA                          mDepthBuffer                {};

  MeshScreen                            mMeshScreenPlane            {};

  TextureR32RGBA                        mTextureNoise               {};

  MeshGizmo&                            mMeshGizmoLineBatch         { RegistryGetOrCreate<MeshGizmo>("meshGizmoLineBatch") };

  u32                                   mVertexOffsetGizmoLineBatch {};
  u32                                   mIndexOffsetGizmoLineBatch  {};
};

/*
* Render passes.
*/

template<typename Renderer> void RendererDispatchLambert(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialLambert);

  while (!renderer.mRenderQueueLambert.empty())
  {
    TaskLambert const& taskLambert{ renderer.mRenderQueueLambert.front() };

    renderer.mUniformBlockProjection.mTransform = taskLambert.mpModel->mTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    TextureLayoutBind(*taskLambert.mpTextureAlbedo);
    TextureLayoutMapSampler(*taskLambert.mpTextureAlbedo, 0);

    for (u32 i{}; i < taskLambert.mpModel->mNumSubMeshes; i++)
    {
      ModelLayoutBind(*taskLambert.mpModel, i);
      ModelLayoutRender(*taskLambert.mpModel, i, eRenderModeTriangle);
    }

    renderer.mRenderQueueLambert.pop();
  }

  ModelLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererDispatchLambertInstanced(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  while (!renderer.mRenderQueueLambertInstanced.empty())
  {
    TaskLambertInstanced const& taskLambertInstanced{ renderer.mRenderQueueLambertInstanced.front() };

    renderer.mUniformBlockProjection.mTransform = taskLambertInstanced.mpModel->mTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    BufferLayoutBind(*taskLambertInstanced.mpBufferTransform);
    BufferLayoutMap(*taskLambertInstanced.mpBufferTransform, 0);

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

  ModelLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
  BufferLayoutUnbind();
}
template<typename Renderer> void RendererDispatchGizmo(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialGizmo);

  MeshLayoutBind(renderer.mMeshGizmoLineBatch);
  MeshLayoutRender(renderer.mMeshGizmoLineBatch, eRenderModeLine);

  MeshLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererDispatchScreen(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformGlobal);
  UniformLayoutMap(renderer.mUniformGlobal, 0);

  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 1);

  UniformLayoutBind(renderer.mUniformCamera);
  UniformLayoutMap(renderer.mUniformCamera, 2);

  TextureLayoutBind(renderer.mTextureNoise);
  TextureLayoutMapSampler(renderer.mTextureNoise, 0);

  ScreenMaterialBind(renderer.mMaterialVolumeCloud);

  MeshLayoutBind(renderer.mMeshScreenPlane);
  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ScreenMaterialUnbind();
  UniformLayoutUnbind();
}

/*
* Renderer management.
*/

template<typename Renderer> void RendererCreate(Renderer& renderer, u32 width, u32 height)
{
  renderer.mWidth = width;
  renderer.mHeight = height;

  UniformLayoutCreate(renderer.mUniformGlobal);
  UniformLayoutCreate(renderer.mUniformProjection);
  UniformLayoutCreate(renderer.mUniformCamera);
  UniformLayoutCreate(renderer.mUniformPointLights);

  RenderMaterialCreate(renderer.mMaterialLambert);
  RenderMaterialCreate(renderer.mMaterialLambertInstanced);
  RenderMaterialCreate(renderer.mMaterialGizmo);

  ScreenMaterialCreate(renderer.mMaterialDeferredLight, "DeferredLightPass");
  ScreenMaterialCreate(renderer.mMaterialVolumeCloud, "VolumeCloud");

  FrameBufferCreate(renderer.mFrameBufferDeferred, renderer.mWidth, renderer.mHeight);

  TextureLayoutCreate(renderer.mTexturePosition, renderer.mWidth, renderer.mHeight, GL_REPEAT, GL_NEAREST);
  TextureLayoutCreate(renderer.mTextureAlbedo, renderer.mWidth, renderer.mHeight, GL_REPEAT, GL_NEAREST);
  TextureLayoutCreate(renderer.mTextureNormal, renderer.mWidth, renderer.mHeight, GL_REPEAT, GL_NEAREST);
  TextureLayoutCreate(renderer.mTextureUv, renderer.mWidth, renderer.mHeight, GL_REPEAT, GL_NEAREST);
  TextureLayoutCreate(renderer.mDepthBuffer, renderer.mWidth, renderer.mHeight, GL_REPEAT, GL_NEAREST);

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

  TextureLayoutCreate(renderer.mTextureNoise, 256, 256, GL_REPEAT, GL_LINEAR);
  TextureLayoutBind(renderer.mTextureNoise);
  TextureLayoutDataSetFrom(renderer.mTextureNoise, SANDBOX_ENGINE_ROOT_PATH "Texture\\RepeatRGBSimplex.png");
  TextureLayoutUnbind();

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
template<typename Renderer> void RendererRenderBegin(Renderer& renderer)
{
  renderer.mUniformBlockGlobal =
  {
    .mTime      { renderer.mTime },
    .mTimeDelta { renderer.mTimeDelta },
    .mScreenSize{ renderer.mWidth, renderer.mHeight },
  };
  renderer.mUniformBlockProjection =
  {
    .mProjection{ renderer.mCamera.mProjection },
    .mView      { renderer.mCamera.mView },
  };
  renderer.mUniformBlockCamera =
  {
    .mPosition  { renderer.mCamera.mPosition },
    .mRotation  { renderer.mCamera.mRotation },
    .mLocalRight{ renderer.mCamera.mLocalRight },
    .mLocalUp   { renderer.mCamera.mLocalUp },
    .mLocalFront{ renderer.mCamera.mLocalFront },
  };
  
  UniformLayoutBind(renderer.mUniformGlobal);
  UniformLayoutDataSet(renderer.mUniformGlobal, 1, &renderer.mUniformBlockGlobal);
  UniformLayoutUnbind();

  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);
  UniformLayoutUnbind();

  UniformLayoutBind(renderer.mUniformCamera);
  UniformLayoutDataSet(renderer.mUniformCamera, 1, &renderer.mUniformBlockCamera);
  UniformLayoutUnbind();

  UniformLayoutBind(renderer.mUniformPointLights);
  UniformLayoutDataSet(renderer.mUniformPointLights, 1, &renderer.mUniformBlockPointLights[0]);
  UniformLayoutUnbind();

  MeshLayoutBind(renderer.mMeshGizmoLineBatch);
  MeshLayoutClear(renderer.mMeshGizmoLineBatch);
  MeshLayoutUnbind();
}
template<typename Renderer> void RendererRender(Renderer& renderer)
{
  SANDBOX_ENGINE_MEASURE_BEGIN(RenderPassDeferredGeometry);

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

  SANDBOX_ENGINE_MEASURE_END(RenderPassDeferredGeometry);

  SANDBOX_ENGINE_MEASURE_BEGIN(RenderPassDeferredLight);

  FrameBufferBindRead(renderer.mFrameBufferDeferred);

  //FrameBufferSetReadTexture(1);
  //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  //TextureLayoutBind(renderer.mTexturePosition);
  //TextureLayoutMapSampler(renderer.mTexturePosition, 0);

  //TextureLayoutBind(renderer.mTextureAlbedo);
  //TextureLayoutMapSampler(renderer.mTextureAlbedo, 1);

  //TextureLayoutBind(renderer.mTextureNormal);
  //TextureLayoutMapSampler(renderer.mTextureNormal, 2);

  //TextureLayoutBind(renderer.mTextureUv);
  //TextureLayoutMapSampler(renderer.mTextureUv, 3);

  //UniformLayoutBind(renderer.mUniformProjection);
  //UniformLayoutMap(renderer.mUniformProjection, 0);

  //UniformLayoutBind(renderer.mUniformPointLight);
  //UniformLayoutMap(renderer.mUniformPointLight, 1);

  //ScreenMaterialBind(renderer.mMaterialDeferredLight);

  FrameBufferUnbind();

  SANDBOX_ENGINE_MEASURE_END(RenderPassDeferredLight);

  SANDBOX_ENGINE_MEASURE_BEGIN(RenderPassBlending);

  RendererDispatchScreen(renderer);

  SANDBOX_ENGINE_MEASURE_END(RenderPassBlending);

  SANDBOX_ENGINE_MEASURE_BEGIN(RenderPassGizmo);

  //RendererDispatchGizmo(renderer);

  SANDBOX_ENGINE_MEASURE_END(RenderPassGizmo);
}
template<typename Renderer> void RendererRenderEnd(Renderer& renderer)
{
  MeshLayoutUnbind();
  ModelLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  ScreenMaterialUnbind();
  UniformLayoutUnbind();
  BufferLayoutUnbind();

  renderer.mVertexOffsetGizmoLineBatch = 0;
  renderer.mIndexOffsetGizmoLineBatch = 0;
}
template<typename Renderer> void RendererDestroy(Renderer const& renderer)
{
  UniformLayoutDestroy(renderer.mUniformGlobal);
  UniformLayoutDestroy(renderer.mUniformProjection);
  UniformLayoutDestroy(renderer.mUniformCamera);
  UniformLayoutDestroy(renderer.mUniformPointLights);

  RenderMaterialDestroy(renderer.mMaterialLambert);
  RenderMaterialDestroy(renderer.mMaterialLambertInstanced);
  RenderMaterialDestroy(renderer.mMaterialGizmo);

  ScreenMaterialDestroy(renderer.mMaterialDeferredLight);
  ScreenMaterialDestroy(renderer.mMaterialVolumeCloud);

  FrameBufferDestroy(renderer.mFrameBufferDeferred);

  TextureLayoutDestroy(renderer.mTexturePosition);
  TextureLayoutDestroy(renderer.mTextureAlbedo);
  TextureLayoutDestroy(renderer.mTextureNormal);
  TextureLayoutDestroy(renderer.mTextureUv);
  TextureLayoutDestroy(renderer.mDepthBuffer);

  MeshLayoutDestroy(renderer.mMeshScreenPlane);
  MeshLayoutDestroy(renderer.mMeshGizmoLineBatch);

  TextureLayoutDestroy(renderer.mTextureNoise);
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
template<typename Renderer> void RendererLineBatchPushBox(Renderer& renderer, r32v3 const& position, r32v3 const& size, r32v4 const& color)
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