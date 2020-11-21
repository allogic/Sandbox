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
  MeshLambert*    mpMesh         {};
  r32m4           mTransform     {};
  TextureR32RGBA* mpTextureAlbedo{};
};
struct TaskLambertInstanced
{
  MeshLambert*             mpMesh           {};
  r32m4                    mTransform       {};
  BufferLayout<Transform>* mpBufferTransform{};
  TextureR32RGBA*          mpTextureAlbedo  {};
  u32                      mNumInstances    {};
};

/*
* Renderer.
*/

struct Renderer
{
  u32                                      mWidth                      {};
  u32                                      mHeight                     {};

  r32 const&                               mTime                       { RegistryGetOrCreate<r32>("time") };
  r32 const&                               mTimeDelta                  { RegistryGetOrCreate<r32>("timeDelta") };
  Camera const&                            mCamera                     { RegistryGetOrCreate<Camera>("camera") };

  UniformBlockGlobal                       mUniformBlockGlobal         {};
  UniformBlockProjection                   mUniformBlockProjection     {};
  UniformBlockCamera                       mUniformBlockCamera         {};
  std::array<UniformBlockPointLight, 1024> mUniformBlockPointLights    {};

  UniformLayout<UniformBlockGlobal>        mUniformGlobal              {};
  UniformLayout<UniformBlockProjection>    mUniformProjection          {};
  UniformLayout<UniformBlockCamera>        mUniformCamera              {};
  UniformLayout<UniformBlockPointLight>    mUniformPointLights         {};

  RenderMaterialLambert                    mMaterialLambert            {};
  RenderMaterialLambertInstanced           mMaterialLambertInstanced   {};
  RenderMaterialGizmo                      mMaterialGizmo              {};

  ScreenMaterialDefault                    mMaterialDeferredLight      {};
  ScreenMaterialDefault                    mMaterialVolumeCloud        {};

  std::queue<TaskLambert>                  mRenderQueueLambert         {};
  std::queue<TaskLambertInstanced>         mRenderQueueLambertInstanced{};

  FrameBuffer                              mFrameBufferDeferred        {};

  TextureR32RGBA                           mTexturePosition            {};
  TextureR32RGBA                           mTextureAlbedo              {};
  TextureR32RGBA                           mTextureNormal              {};
  TextureR32RGBA                           mTextureUv                  {};
  TextureR32Depth                          mTextureDepth               {};

  MeshScreen                               mMeshScreenPlane            {};

  TextureR32RGBA                           mTextureNoise               {};

  MeshGizmo&                               mMeshGizmoLineBatch         { RegistryGetOrCreate<MeshGizmo>("meshGizmoLineBatch") };

  u32                                      mVertexOffsetGizmoLineBatch {};
  u32                                      mIndexOffsetGizmoLineBatch  {};
};

/*
* Render passes.
*/

template<typename Renderer> void RendererDispatchLambert(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialLambert);

  while (!renderer.mRenderQueueLambert.empty())
  {
    TaskLambert const& taskLambert{ renderer.mRenderQueueLambert.front() };

    renderer.mUniformBlockProjection.mTransform = taskLambert.mTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    TextureLayoutMapSampler(*taskLambert.mpTextureAlbedo, 0);

    MeshLayoutRender(*taskLambert.mpMesh, eRenderModeTriangle);

    renderer.mRenderQueueLambert.pop();
  }

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererDispatchLambertInstanced(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 0);

  while (!renderer.mRenderQueueLambertInstanced.empty())
  {
    TaskLambertInstanced const& taskLambertInstanced{ renderer.mRenderQueueLambertInstanced.front() };

    renderer.mUniformBlockProjection.mTransform = taskLambertInstanced.mTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    BufferLayoutMap(*taskLambertInstanced.mpBufferTransform, 0);

    TextureLayoutMapSampler(*taskLambertInstanced.mpTextureAlbedo, 0);

    RenderMaterialBind(renderer.mMaterialLambertInstanced);

    MeshLayoutRenderInstanced(*taskLambertInstanced.mpMesh, eRenderModeTriangle, taskLambertInstanced.mNumInstances);

    renderer.mRenderQueueLambertInstanced.pop();
  }

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
  BufferLayoutUnbind();
}
template<typename Renderer> void RendererDispatchLambertLight(Renderer& renderer)
{
  TextureLayoutMapSampler(renderer.mTexturePosition, 0);
  TextureLayoutMapSampler(renderer.mTextureAlbedo, 1);
  TextureLayoutMapSampler(renderer.mTextureNormal, 2);
  TextureLayoutMapSampler(renderer.mTextureUv, 3);

  UniformLayoutMap(renderer.mUniformProjection, 0);
  UniformLayoutMap(renderer.mUniformCamera, 1);
  UniformLayoutMap(renderer.mUniformPointLights, 2);

  ScreenMaterialBind(renderer.mMaterialDeferredLight);

  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ScreenMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererDispatchGizmo(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialGizmo);

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
  UniformLayoutCreate(renderer.mUniformPointLights, (u32)renderer.mUniformBlockPointLights.size());

  RenderMaterialCreate(renderer.mMaterialLambert);
  RenderMaterialCreate(renderer.mMaterialLambertInstanced);
  RenderMaterialCreate(renderer.mMaterialGizmo);

  ScreenMaterialCreate(renderer.mMaterialDeferredLight, "DeferredLightPass");
  ScreenMaterialCreate(renderer.mMaterialVolumeCloud, "VolumeCloud");

  FrameBufferCreate(renderer.mFrameBufferDeferred, renderer.mWidth, renderer.mHeight);

  TextureLayoutCreate(renderer.mTexturePosition, renderer.mWidth, renderer.mHeight, eTextureWrapRepeat, eTextureFilterNearest);
  TextureLayoutCreate(renderer.mTextureAlbedo, renderer.mWidth, renderer.mHeight, eTextureWrapRepeat, eTextureFilterNearest);
  TextureLayoutCreate(renderer.mTextureNormal, renderer.mWidth, renderer.mHeight, eTextureWrapRepeat, eTextureFilterNearest);
  TextureLayoutCreate(renderer.mTextureUv, renderer.mWidth, renderer.mHeight, eTextureWrapRepeat, eTextureFilterNearest);
  TextureLayoutCreate(renderer.mTextureDepth, renderer.mWidth, renderer.mHeight, eTextureWrapRepeat, eTextureFilterNearest);

  std::vector<VertexScreen> screenVertices
  {
    { { -1.f, -1.f, 0.f }, { 0.f, 0.f } },
    { {  1.f, -1.f, 0.f }, { 1.f, 0.f } },
    { { -1.f,  1.f, 0.f }, { 0.f, 1.f } },
    { {  1.f,  1.f, 0.f }, { 1.f, 1.f } },
  };
  std::vector<u32> screenIndices{ 0, 1, 2, 2, 3, 1 };
  std::vector<u32> screenVertexBufferSizes{ 4 };
  std::vector<u32> screenIndexBufferSizes{ 6 };
  MeshLayoutCreate(renderer.mMeshScreenPlane, 1, screenVertexBufferSizes.data(), screenIndexBufferSizes.data());
  MeshLayoutBind(renderer.mMeshScreenPlane, 0);
  MeshLayoutData(renderer.mMeshScreenPlane, 0, screenVertices.data(), screenIndices.data());
  MeshLayoutUnbind();

  std::vector<u32> gizmoVertexBufferSizes{ 65535 };
  std::vector<u32> gizmoIndexBufferSizes{ 65535 * 2 };
  MeshLayoutCreate(renderer.mMeshGizmoLineBatch, 1, gizmoVertexBufferSizes.data(), gizmoIndexBufferSizes.data());

  TextureFrom(renderer.mTextureNoise, SANDBOX_ENGINE_ROOT_PATH "Texture\\Noise.png");

  FrameBufferBindWrite(renderer.mFrameBufferDeferred);

  FrameBufferAttachTexture(renderer.mTexturePosition, 0);
  FrameBufferAttachTexture(renderer.mTextureAlbedo, 1);
  FrameBufferAttachTexture(renderer.mTextureNormal, 2);
  FrameBufferAttachTexture(renderer.mTextureUv, 3);

  FrameBufferAttachDepthBuffer(renderer.mTextureDepth);

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

  renderer.mUniformBlockPointLights[0] =
  {
    .mPosition            { glm::sin(renderer.mTime * 0.3f) * 100, 30.f, glm::cos(renderer.mTime * 0.3f) * 100 },
    .mRadius              { 100.f },
    .mColor               { 0.f, 0.f, 1.f, 1.f },
    .mEnabled             { 1 },
    .mAttenuationLinear   { 0.f },
    .mAttenuationQuadratic{ 0.003f },
  };
  renderer.mUniformBlockPointLights[1] =
  {
    .mPosition            { glm::cos(renderer.mTime * 0.3f) * 100, 30.f, glm::sin(renderer.mTime * 0.3f) * 100 },
    .mRadius              { 100.f },
    .mColor               { 1.f, 0.f, 0.f, 1.f },
    .mEnabled             { 1 },
    .mAttenuationLinear   { 0.f },
    .mAttenuationQuadratic{ 0.003f },
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
  UniformLayoutDataSet(renderer.mUniformPointLights, (u32)renderer.mUniformBlockPointLights.size(), renderer.mUniformBlockPointLights.data());
  UniformLayoutUnbind();

  MeshLayoutBind(renderer.mMeshGizmoLineBatch, 0);
  MeshLayoutClear(renderer.mMeshGizmoLineBatch, 0);
  MeshLayoutUnbind();
}
template<typename Renderer> void RendererRender(Renderer& renderer)
{
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

  FrameBufferBindRead(renderer.mFrameBufferDeferred);

  //FrameBufferSetReadTexture(0);
  //glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  RendererDispatchLambertLight(renderer);

  FrameBufferUnbind();

  //RendererDispatchScreen(renderer);

  //RendererDispatchGizmo(renderer);
}
template<typename Renderer> void RendererRenderEnd(Renderer& renderer)
{
  MeshLayoutUnbind();
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
  TextureLayoutDestroy(renderer.mTextureDepth);

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
  u32 indices[2]{ renderer.mVertexOffsetGizmoLineBatch + 0, renderer.mVertexOffsetGizmoLineBatch + 1 };

  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, 0, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 2, 2);

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

  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, 0, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 8, 24);

  renderer.mVertexOffsetGizmoLineBatch += 8;
  renderer.mIndexOffsetGizmoLineBatch += 24;
}