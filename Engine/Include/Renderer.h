#pragma once

#include <Core.h>
#include <Types.h>
#include <Registry.h>
#include <Camera.h>

#include <FrameBuffers/FrameBuffer.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>
#include <Layouts/TextureLayouts.h>

#include <Materials/RenderMaterials.h>
#include <Materials/ScreenMaterials.h>

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/LambertUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>

#include <Buffers/TransformBuffers.h>

/*
* Render tasks.
*/

struct TaskLambert
{
  MeshLambert*    mpMesh            {};
  r32m4           mTransformModel   {};
  TextureR32RGBA* mpTextureAlbedo   {};
  TextureR32RGBA* mpTextureNormal   {};
  TextureR32RGBA* mpTextureSpecular {};
  TextureR32RGBA* mpTextureMetallic {};
  TextureR32RGBA* mpTextureRoughness{};
};
struct TaskLambertInstanced
{
  MeshLambert*                   mpMesh            {};
  r32m4                          mTransformModel   {};
  BufferLayout<BufferTransform>* mpBufferTransform {};
  TextureR32RGBA*                mpTextureAlbedo   {};
  TextureR32RGBA*                mpTextureNormal   {};
  TextureR32RGBA*                mpTextureSpecular {};
  TextureR32RGBA*                mpTextureMetallic {};
  TextureR32RGBA*                mpTextureRoughness{};
  u32                            mNumInstances     {};
};

/*
* Renderer.
*/

struct Renderer
{
  u32                                    mWidth                      {};
  u32                                    mHeight                     {};

  r32 const&                             mTime                       { RegistryGetOrCreate<r32>("time") };
  r32 const&                             mTimeDelta                  { RegistryGetOrCreate<r32>("timeDelta") };
  Camera const&                          mCamera                     { RegistryGetOrCreate<Camera>("camera") };

  UniformBlockGlobal                     mUniformBlockGlobal         {};
  UniformBlockProjection                 mUniformBlockProjection     {};
  UniformBlockCamera                     mUniformBlockCamera         {};
  UniformBlockLambert                    mUniformBlockLambert        {};

  std::array<UniformBlockPointLight, 32> mUniformBlockPointLights    {};

  UniformLayout<UniformBlockGlobal>      mUniformGlobal              {};
  UniformLayout<UniformBlockProjection>  mUniformProjection          {};
  UniformLayout<UniformBlockCamera>      mUniformCamera              {};
  UniformLayout<UniformBlockLambert>     mUniformLambert             {};

  UniformLayout<UniformBlockPointLight>  mUniformPointLights         {};

  RenderMaterialLambert                  mMaterialLambert            {};
  RenderMaterialLambertInstanced         mMaterialLambertInstanced   {};
  RenderMaterialGizmo                    mMaterialGizmo              {};

  ScreenMaterialDefault                  mMaterialDeferredLight      {};
  ScreenMaterialDefault                  mMaterialVolumeCloud        {};

  std::queue<TaskLambert>                mRenderQueueLambert         {};
  std::queue<TaskLambertInstanced>       mRenderQueueLambertInstanced{};

  FrameBufferDeferred                    mFrameBufferDeferred        {};

  MeshScreen                             mMeshScreenPlane            {};

  TextureR32RGBA                         mTextureNoise               {};

  MeshGizmo&                             mMeshGizmoLineBatch         { RegistryGetOrCreate<MeshGizmo>("meshGizmoLineBatch") };

  u32                                    mVertexOffsetGizmoLineBatch {};
  u32                                    mIndexOffsetGizmoLineBatch  {};
};

/*
* Render passes.
*/

template<typename Renderer> void RendererPassGeometry(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 1);

  RenderMaterialBind(renderer.mMaterialLambert);

  while (!renderer.mRenderQueueLambert.empty())
  {
    TaskLambert const& task{ renderer.mRenderQueueLambert.front() };

    renderer.mUniformBlockProjection.mTransformModel = task.mTransformModel;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    for (u32 i{}; i < task.mpMesh->mNumSubMeshes; i++)
    {
      if (task.mpTextureAlbedo) TextureLayoutMapSampler(*task.mpTextureAlbedo, 0);
      if (task.mpTextureNormal) TextureLayoutMapSampler(*task.mpTextureNormal, 1);
      if (task.mpTextureSpecular) TextureLayoutMapSampler(*task.mpTextureSpecular, 2);
      if (task.mpTextureMetallic) TextureLayoutMapSampler(*task.mpTextureMetallic, 3);
      if (task.mpTextureRoughness) TextureLayoutMapSampler(*task.mpTextureRoughness, 4);

      MeshLayoutBind(*task.mpMesh, i);
      MeshLayoutRenderPartial(*task.mpMesh, i, eRenderModeTriangle);
    }

    renderer.mRenderQueueLambert.pop();
  }

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassGeometryInstanced(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 0);

  while (!renderer.mRenderQueueLambertInstanced.empty())
  {
    TaskLambertInstanced const& task{ renderer.mRenderQueueLambertInstanced.front() };

    renderer.mUniformBlockProjection.mTransformModel = task.mTransformModel;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    BufferLayoutMap(*task.mpBufferTransform, 0);

    RenderMaterialBind(renderer.mMaterialLambertInstanced);

    for (u32 i{}; i < task.mpMesh->mNumSubMeshes; i++)
    {
      if (task.mpTextureAlbedo) TextureLayoutMapSampler(*task.mpTextureAlbedo, 0);
      if (task.mpTextureNormal) TextureLayoutMapSampler(*task.mpTextureNormal, 1);
      if (task.mpTextureSpecular) TextureLayoutMapSampler(*task.mpTextureSpecular, 2);
      if (task.mpTextureMetallic) TextureLayoutMapSampler(*task.mpTextureMetallic, 3);
      if (task.mpTextureRoughness) TextureLayoutMapSampler(*task.mpTextureRoughness, 4);

      MeshLayoutBind(*task.mpMesh, i);
      MeshLayoutRenderInstancedPartial(*task.mpMesh, i, eRenderModeTriangle, task.mNumInstances);
    }

    renderer.mRenderQueueLambertInstanced.pop();
  }

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
  BufferLayoutUnbind();
}
template<typename Renderer> void RendererPassLight(Renderer& renderer)
{
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTexturePosition, 0);
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTextureAlbedo, 1);
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTextureNormal, 2);
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTextureSpecular, 3);
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTextureMetallic, 4);
  TextureLayoutMapSampler(renderer.mFrameBufferDeferred.mTextureRoughness, 5);
  TextureLayoutMapSampler(renderer.mTextureNoise, 6);

  UniformLayoutMap(renderer.mUniformGlobal, 0);
  UniformLayoutMap(renderer.mUniformProjection, 1);
  UniformLayoutMap(renderer.mUniformCamera, 2);
  UniformLayoutMap(renderer.mUniformPointLights, 3);
  UniformLayoutMap(renderer.mUniformLambert, 4);

  ScreenMaterialBind(renderer.mMaterialDeferredLight);

  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ScreenMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassScreen(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformGlobal, 0);
  UniformLayoutMap(renderer.mUniformProjection, 1);
  UniformLayoutMap(renderer.mUniformCamera, 2);

  TextureLayoutMapSampler(renderer.mTextureNoise, 0);

  ScreenMaterialBind(renderer.mMaterialVolumeCloud);

  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ScreenMaterialUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassGizmo(Renderer& renderer)
{
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutMap(renderer.mUniformProjection, 0);

  RenderMaterialBind(renderer.mMaterialGizmo);

  MeshLayoutBind(renderer.mMeshGizmoLineBatch, 0);
  MeshLayoutRenderPartial(renderer.mMeshGizmoLineBatch, 0, eRenderModeLine);

  MeshLayoutUnbind();
  RenderMaterialUnbind();
  UniformLayoutUnbind();
}

/*
* Render commands.
*/

template<typename Renderer> void RendererSubmitLambert(Renderer& renderer, TaskLambert const& task)
{
  renderer.mRenderQueueLambert.emplace(task);
}
template<typename Renderer> void RendererSubmitLambertInstanced(Renderer& renderer, TaskLambertInstanced const& task)
{
  renderer.mRenderQueueLambertInstanced.emplace(task);
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
  UniformLayoutCreate(renderer.mUniformLambert);

  UniformLayoutCreate(renderer.mUniformPointLights, (u32)renderer.mUniformBlockPointLights.size());

  RenderMaterialCreate(renderer.mMaterialLambert);
  RenderMaterialCreate(renderer.mMaterialLambertInstanced);
  RenderMaterialCreate(renderer.mMaterialGizmo);

  ScreenMaterialCreate(renderer.mMaterialDeferredLight, "DeferredLightPass");
  ScreenMaterialCreate(renderer.mMaterialVolumeCloud, "VolumeCloud");

  FrameBufferCreate(renderer.mFrameBufferDeferred, renderer.mWidth, renderer.mHeight);

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

  TextureFrom(renderer.mTextureNoise, SANDBOX_ROOT_PATH "Textures\\Noise.png");
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
    .mProjection     { renderer.mCamera.mProjection },
    .mView           { renderer.mCamera.mView },
    .mTransformCamera{ renderer.mCamera.mTransform },
  };
  renderer.mUniformBlockCamera =
  {
    .mPosition  { renderer.mCamera.mPosition },
    .mRotation  { renderer.mCamera.mRotation },
    .mRight     { renderer.mCamera.mRight },
    .mUp        { renderer.mCamera.mUp },
    .mFront     { renderer.mCamera.mFront },
    .mLocalRight{ renderer.mCamera.mLocalRight },
    .mLocalUp   { renderer.mCamera.mLocalUp },
    .mLocalFront{ renderer.mCamera.mLocalFront },
  };

  renderer.mUniformBlockPointLights[0] =
  {
    .mPosition            { 0.f, 0.f, 0.f },
    .mRadius              { 600.f },
    .mColor               { 1.f, 1.f, 1.f, 1.f },
    .mEnabled             { 1 },
    .mAttenuationLinear   { 0.f },
    .mAttenuationQuadratic{ 0.001f },
  };
  renderer.mUniformBlockPointLights[1] =
  {
    .mPosition            { glm::sin(renderer.mTime * 0.3f) * 100, 0.f, glm::cos(renderer.mTime * 0.3f) * 100 },
    .mRadius              { 300.f },
    .mColor               { 0.f, 0.f, 1.f, 1.f },
    .mEnabled             { 1 },
    .mAttenuationLinear   { 0.f },
    .mAttenuationQuadratic{ 0.003f },
  };
  renderer.mUniformBlockPointLights[2] =
  {
    .mPosition            { glm::cos(renderer.mTime * 0.3f) * 100, 0.f, glm::sin(renderer.mTime * 0.3f) * 100 },
    .mRadius              { 300.f },
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
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Geometry pass
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  RendererPassGeometry(renderer);
  RendererPassGeometryInstanced(renderer);
  //glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Light pass
  glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  RendererPassLight(renderer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Copy geometry depth into default framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Render background blend geometry
  //glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  //RendererDispatchScreen(renderer);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Gizmo pass
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glEnable(GL_DEPTH_TEST);
  RendererPassGizmo(renderer);
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
  UniformLayoutDestroy(renderer.mUniformLambert);

  RenderMaterialDestroy(renderer.mMaterialLambert);
  RenderMaterialDestroy(renderer.mMaterialLambertInstanced);
  RenderMaterialDestroy(renderer.mMaterialGizmo);

  ScreenMaterialDestroy(renderer.mMaterialDeferredLight);
  ScreenMaterialDestroy(renderer.mMaterialVolumeCloud);

  FrameBufferDestroy(renderer.mFrameBufferDeferred);

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

/*
* Renderer render texture management.
*/

template<typename Renderer, typename TextureLayout> void RendererRenderTextureGet(Renderer const& renderer, TextureLayout const& textureLayout)
{

}