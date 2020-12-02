#pragma once

#include <Core.h>
#include <Types.h>
#include <Registry.h>
#include <ACS.h>
#include <Utility.h>

#include <FrameBuffers/FrameBufferDeferred.h>

#include <Components/VertexComponents.h>
#include <Components/UniformComponents.h>
#include <Components/BufferComponents.h>
#include <Components/TextureComponents.h>
#include <Components/ShaderComponents.h>
#include <Components/CameraComponents.h>
#include <Components/LightComponents.h>
#include <Components/RenderComponents.h>

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/LambertUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>

/*
* Renderer.
*/

struct Renderer
{
  u32                                    mWidth                      {};
  u32                                    mHeight                     {};

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

  ShaderLambert                          mShaderLambert              {};
  ShaderLambertInstanced                 mShaderLambertInstanced     {};
  ShaderGizmo                            mShaderGizmo                {};

  ShaderScreen                           mShaderDeferredLight        {};
  ShaderScreen                           mShaderVolumeCloud          {};

  FrameBufferDeferred                    mFrameBufferDeferred        {};

  MeshScreen                             mMeshScreenPlane            {};

  TextureR32RGBA                         mTextureNoise               {};

  r32m4                                  mTransformGizmo             {};

  MeshGizmo                              mMeshGizmoLineBatch         {};

  u32                                    mVertexOffsetGizmoLineBatch {};
  u32                                    mIndexOffsetGizmoLineBatch  {};
};

/*
* Render passes.
*/

template<typename Renderer> void RendererPassGeometry(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 0);

  ShaderLayoutBind(renderer.mShaderLambert);

  ACS::DispatchFor<Renderable>([&](Actor* pActor, Renderable* pRenderable)
  {
    r32m4 worldTransform{ glm::identity<r32m4>() };

    if (pActor->mpParent)
    {
      worldTransform = TransformTo(pActor->mpParent->mTransform.mPosition, pActor->mpParent->mTransform.mRotation, pActor->mpParent->mTransform.mScale);
    }
    
    r32m4 localTransform{ TransformTo(pActor->mTransform.mPosition, pActor->mTransform.mRotation, pActor->mTransform.mScale) };

    renderer.mUniformBlockProjection.mTransformModel = worldTransform * localTransform;
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    for (u32 i{}; i < pRenderable->mpMeshLambert->mNumSubMeshes; i++)
    {
      if (pRenderable->mpTextureAlbedo) TextureLayoutMapSampler(*pRenderable->mpTextureAlbedo, 0);
      if (pRenderable->mpTextureNormal) TextureLayoutMapSampler(*pRenderable->mpTextureNormal, 1);
      if (pRenderable->mpTextureSpecular) TextureLayoutMapSampler(*pRenderable->mpTextureSpecular, 2);
      if (pRenderable->mpTextureMetallic) TextureLayoutMapSampler(*pRenderable->mpTextureMetallic, 3);
      if (pRenderable->mpTextureRoughness) TextureLayoutMapSampler(*pRenderable->mpTextureRoughness, 4);

      MeshLayoutBind(*pRenderable->mpMeshLambert, i);
      MeshLayoutRenderPartial(*pRenderable->mpMeshLambert, i, eRenderModeTriangle);
    }
  });

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ShaderLayoutUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassGeometryInstanced(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformProjection, 0);

  ACS::DispatchFor<RenderableInstanced>([&](Actor* pActor, RenderableInstanced* pRenderable)
  {
    renderer.mUniformBlockProjection.mTransformModel = glm::identity<r32m4>();
    UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

    BufferLayoutMap(*pRenderable->mpBufferTransform, 0);

    ShaderLayoutBind(renderer.mShaderLambertInstanced);

    for (u32 i{}; i < pRenderable->mpMeshLambert->mNumSubMeshes; i++)
    {
      if (pRenderable->mpTextureAlbedo) TextureLayoutMapSampler(*pRenderable->mpTextureAlbedo, 0);
      if (pRenderable->mpTextureNormal) TextureLayoutMapSampler(*pRenderable->mpTextureNormal, 1);
      if (pRenderable->mpTextureSpecular) TextureLayoutMapSampler(*pRenderable->mpTextureSpecular, 2);
      if (pRenderable->mpTextureMetallic) TextureLayoutMapSampler(*pRenderable->mpTextureMetallic, 3);
      if (pRenderable->mpTextureRoughness) TextureLayoutMapSampler(*pRenderable->mpTextureRoughness, 4);

      MeshLayoutBind(*pRenderable->mpMeshLambert, i);
      MeshLayoutRenderInstancedPartial(*pRenderable->mpMeshLambert, i, eRenderModeTriangle, pRenderable->mpBufferTransform->mBufferSize);
    }
  });

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ShaderLayoutUnbind();
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

  ShaderLayoutBind(renderer.mShaderDeferredLight);

  MeshLayoutBind(renderer.mMeshScreenPlane, 0);
  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ShaderLayoutUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassScreen(Renderer& renderer)
{
  UniformLayoutMap(renderer.mUniformGlobal, 0);
  UniformLayoutMap(renderer.mUniformProjection, 1);
  UniformLayoutMap(renderer.mUniformCamera, 2);

  TextureLayoutMapSampler(renderer.mTextureNoise, 0);

  ShaderLayoutBind(renderer.mShaderVolumeCloud);

  MeshLayoutRender(renderer.mMeshScreenPlane, eRenderModeTriangle);

  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ShaderLayoutUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassGizmo(Renderer& renderer)
{
  MeshLayoutBind(renderer.mMeshGizmoLineBatch, 0);

  RendererLineBatchPopMatrix(renderer);
  RendererLineBatchPushLine(renderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
  RendererLineBatchPushLine(renderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
  RendererLineBatchPushLine(renderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });

  ACS::Dispatch([=](Actor* pActor)
  {
    pActor->OnGizmo();
  });

  UniformLayoutMap(renderer.mUniformProjection, 0);

  r32m4 worldTransform{ TransformTo({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }) };

  renderer.mUniformBlockProjection.mTransformModel = worldTransform;
  UniformLayoutDataSet(renderer.mUniformProjection, 1, &renderer.mUniformBlockProjection);

  ShaderLayoutBind(renderer.mShaderGizmo);

  MeshLayoutBind(renderer.mMeshGizmoLineBatch, 0);
  MeshLayoutRenderPartial(renderer.mMeshGizmoLineBatch, 0, eRenderModeLine);

  MeshLayoutUnbind();
  ShaderLayoutUnbind();
  UniformLayoutUnbind();
}
template<typename Renderer> void RendererPassImGui(Renderer& renderer)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();
  ACS::Dispatch([=](Actor* pActor)
  {
    pActor->OnImGui();
  });
  ImGui::EndFrame();
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

  ShaderLayoutCreate(renderer.mShaderLambert, ShaderPaths
  {
    .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" },
    .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" }
  });
  ShaderLayoutCreate(renderer.mShaderLambertInstanced, ShaderPaths
  {
    .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\LambertInstanced.vert" },
    .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" },
  });
  ShaderLayoutCreate(renderer.mShaderGizmo, ShaderPaths
  {
    .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.vert" },
    .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.frag" }
  });

  ShaderLayoutCreate(renderer.mShaderDeferredLight, ShaderPaths
  {
    .mVertex  { SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\Screen.vert" },
    .mFragment{ std::string{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\DeferredLightPass.frag" } },
  });
  ShaderLayoutCreate(renderer.mShaderVolumeCloud, ShaderPaths
  {
    .mVertex  { SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\Screen.vert" },
    .mFragment{ std::string{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\CloudVolumePass.frag" } },
  });

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

  std::vector<u32> gizmoVertexBufferSizes{ 131070 };
  std::vector<u32> gizmoIndexBufferSizes{ 131070 * 2 };
  MeshLayoutCreate(renderer.mMeshGizmoLineBatch, 1, gizmoVertexBufferSizes.data(), gizmoIndexBufferSizes.data());

  TextureFrom(renderer.mTextureNoise, SANDBOX_ROOT_PATH "Textures\\Noise.png");
}
template<typename Renderer> void RendererRenderBegin(Renderer& renderer, r32 time, r32 timeDelta)
{
  // update global parameters
  renderer.mUniformBlockGlobal =
  {
    .mTime      { time },
    .mTimeDelta { timeDelta },
    .mScreenSize{ renderer.mWidth, renderer.mHeight },
  };

  // update camera parameters
  ACS::DispatchFor<Camera>([&](Actor* pActor, Camera* pCamera)
  {
    r32m4 worldTransform{ glm::identity<r32m4>() };

    if (pActor->mpParent)
    {
      worldTransform = TransformTo(pActor->mpParent->mTransform.mPosition, pActor->mpParent->mTransform.mRotation, pActor->mpParent->mTransform.mScale);
    }

    r32m4 localTransform{ TransformTo(pActor->mTransform.mPosition, {}, pActor->mTransform.mScale) };

    r32m4 r0{ glm::identity<r32m4>() };
    r0 = glm::rotate(r0, pActor->mpParent->mTransform.mRotation.x, { 1.f, 0.f, 0.f });
    r0 = glm::rotate(r0, pActor->mpParent->mTransform.mRotation.y, { 0.f, 1.f, 0.f });
    r0 = glm::rotate(r0, pActor->mpParent->mTransform.mRotation.z, { 0.f, 0.f, 1.f });

    renderer.mUniformBlockProjection =
    {
      .mProjection     { glm::perspective(pCamera->mFovRad, pCamera->mAspect, pCamera->mNear, pCamera->mFar) },
      .mView           { localTransform * glm::inverse(worldTransform) },
      .mTransformCamera{ glm::identity<r32m4>() },
    };
    renderer.mUniformBlockCamera =
    {
      .mPosition  { 0.f, 0.f, 0.f },
      .mRotation  { 0.f, 0.f, 0.f },
      .mRight     { 1.f, 0.f, 0.f },
      .mUp        { 0.f, 1.f, 0.f },
      .mFront     { 0.f, 0.f, 1.f },
      .mLocalRight{ 1.f, 0.f, 0.f },
      .mLocalUp   { 0.f, 1.f, 0.f },
      .mLocalFront{ 0.f, 0.f, 1.f },
    };
  });

  // update light parameters
  ACS::DispatchFor<LightPoint>([&](Actor* pActor, LightPoint* pLightPoint)
  {
    renderer.mUniformBlockPointLights[0] =
    {
      .mPosition            { 0.f, 0.f, 0.f },
      .mRadius              { 600.f },
      .mColor               { 1.f, 1.f, 1.f, 1.f },
      .mEnabled             { 1 },
      .mAttenuationLinear   { 0.f },
      .mAttenuationQuadratic{ 0.001f },
    };
  });

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
  //RendererPassGizmo(renderer);
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // ImGui pass
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  RendererPassImGui(renderer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
template<typename Renderer> void RendererRenderEnd(Renderer& renderer)
{
  MeshLayoutUnbind();
  TextureLayoutUnbind();
  ShaderLayoutUnbind();
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

  ShaderLayoutDestroy(renderer.mShaderLambert);
  ShaderLayoutDestroy(renderer.mShaderLambertInstanced);
  ShaderLayoutDestroy(renderer.mShaderGizmo);

  ShaderLayoutDestroy(renderer.mShaderDeferredLight);
  ShaderLayoutDestroy(renderer.mShaderVolumeCloud);

  FrameBufferDestroy(renderer.mFrameBufferDeferred);

  MeshLayoutDestroy(renderer.mMeshScreenPlane);
  MeshLayoutDestroy(renderer.mMeshGizmoLineBatch);

  TextureLayoutDestroy(renderer.mTextureNoise);
}

/*
* Renderer gizmo management.
*/

template<typename Renderer> void RendererLineBatchPushMatrix(Renderer& renderer, r32m4 const& transform)
{
  renderer.mTransformGizmo = transform;
}
template<typename Renderer> void RendererLineBatchPopMatrix(Renderer& renderer)
{
  renderer.mTransformGizmo = glm::identity<r32m4>();
}
template<typename Renderer> void RendererLineBatchPushLine(Renderer& renderer, r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  r32v3 v0{ renderer.mTransformGizmo * r32v4{ p0, 1.f } };
  r32v3 v1{ renderer.mTransformGizmo * r32v4{ p1, 1.f } };

  VertexGizmo vertices[2]{ { v0, color }, { v1, color } };
  u32 indices[2]{ renderer.mVertexOffsetGizmoLineBatch + 0, renderer.mVertexOffsetGizmoLineBatch + 1 };

  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, 0, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 2, 2);

  renderer.mVertexOffsetGizmoLineBatch += 2;
  renderer.mIndexOffsetGizmoLineBatch += 2;
}
template<typename Renderer> void RendererLineBatchPushBox(Renderer& renderer, r32v3 const& position, r32v3 const& size, r32v4 const& color)
{
  r32v3 half{ size };

  r32v3 blf{ renderer.mTransformGizmo * r32v4{ -half.x, -half.y, -half.z, 1.f } };
  r32v3 brf{ renderer.mTransformGizmo * r32v4{ half.x, -half.y, -half.z, 1.f } };
  r32v3 tlf{ renderer.mTransformGizmo * r32v4{ -half.x,  half.y, -half.z, 1.f } };
  r32v3 trf{ renderer.mTransformGizmo * r32v4{ half.x,  half.y, -half.z, 1.f } };

  r32v3 blb{ renderer.mTransformGizmo * r32v4{ -half.x, -half.y,  half.z, 1.f } };
  r32v3 brb{ renderer.mTransformGizmo * r32v4{ half.x, -half.y,  half.z, 1.f } };
  r32v3 tlb{ renderer.mTransformGizmo * r32v4{ -half.x,  half.y,  half.z, 1.f } };
  r32v3 trb{ renderer.mTransformGizmo * r32v4{ half.x,  half.y,  half.z, 1.f } };

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