#pragma once

#include <Core.h>
#include <Types.h>

#include <FrameBuffer.h>
#include <DepthBuffer.h>

#include <Layouts/TextureLayouts.h>

/*
* Graphic buffers.
*/

struct GraphicBuffer
{
  u32            mWidth          {};
  u32            mHeight         {};
  FrameBuffer    mFrameBuffer    {};
  DepthBuffer    mDepthBuffer    {};
  TextureR32RGBA mTexturePosition{};
  TextureR32RGBA mTextureAlbedo  {};
  TextureR32RGBA mTextureNormal  {};
  TextureR32RGBA mTextureUv      {};
};

/*
* GraphicBuffer management.
*/

template<typename GraphicBuffer> void GraphicBufferCreate(GraphicBuffer& graphicBuffer, u32 width, u32 height)
{
  graphicBuffer.mWidth = width;
  graphicBuffer.mHeight = height;

  FrameBufferCreate(graphicBuffer.mFrameBuffer);
  DepthBufferCreate(graphicBuffer.mDepthBuffer, graphicBuffer.mWidth, graphicBuffer.mHeight);

  TextureLayoutCreate(graphicBuffer.mTexturePosition, graphicBuffer.mWidth, graphicBuffer.mHeight);
  TextureLayoutCreate(graphicBuffer.mTextureAlbedo, graphicBuffer.mWidth, graphicBuffer.mHeight);
  TextureLayoutCreate(graphicBuffer.mTextureNormal, graphicBuffer.mWidth, graphicBuffer.mHeight);
  TextureLayoutCreate(graphicBuffer.mTextureUv, graphicBuffer.mWidth, graphicBuffer.mHeight);

  FrameBufferBindWrite(graphicBuffer.mFrameBuffer);

  FrameBufferAttachTexture(graphicBuffer.mTexturePosition, 0);
  FrameBufferAttachTexture(graphicBuffer.mTextureAlbedo, 1);
  FrameBufferAttachTexture(graphicBuffer.mTextureNormal, 2);
  FrameBufferAttachTexture(graphicBuffer.mTextureUv, 3);

  FrameBufferAttachDepthBuffer(graphicBuffer.mDepthBuffer);

  u32 buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

  glDrawBuffers(4, buffers);

  FrameBufferUnbindWrite(graphicBuffer.mFrameBuffer);
}
template<typename GraphicBuffer> void GraphicBufferReadBuffer(GraphicBuffer const& graphicBuffer, u32 textureIndex)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
}
template<typename GraphicBuffer> void GraphicBufferDestroy(GraphicBuffer const& graphicBuffer)
{
  FrameBufferDestroy(graphicBuffer.mFrameBuffer);
  DepthBufferDestroy(graphicBuffer.mDepthBuffer);

  TextureLayoutDestroy(graphicBuffer.mTexturePosition);
  TextureLayoutDestroy(graphicBuffer.mTextureAlbedo);
  TextureLayoutDestroy(graphicBuffer.mTextureNormal);
  TextureLayoutDestroy(graphicBuffer.mTextureUv);
}

/*
* GraphicBuffer render passes.
*/

template<typename GraphicBuffer> void GraphicBufferDeferredPassGeometryBegin(GraphicBuffer const& graphicBuffer)
{
  FrameBufferBindWrite(graphicBuffer.mFrameBuffer);

  glDepthMask(GL_TRUE);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}
template<typename GraphicBuffer> void GraphicBufferDeferredPassGeometryEnd(GraphicBuffer const& graphicBuffer)
{
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  FrameBufferUnbind(graphicBuffer.mFrameBuffer);
}
template<typename GraphicBuffer> void GraphicBufferDeferredPassLightBegin(GraphicBuffer const& graphicBuffer)
{
  FrameBufferBindRead(graphicBuffer.mFrameBuffer);

  //glClearColor(0.f, 0.f, 0.f, 1.f);
  //glClear(GL_COLOR_BUFFER_BIT);

  u32 width{ graphicBuffer.mWidth };
  u32 height{ graphicBuffer.mHeight };
  u32 halfWidth{ (u32)(width / 2.f) };
  u32 halfHeight{ (u32)(height / 2.f) };

  GraphicBufferReadBuffer(graphicBuffer, 1);
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  //GraphicBufferReadBuffer(sGraphicBuffer, 0);
  //glBlitFramebuffer(0, 0, width, height, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(sGraphicBuffer, 1);
  //glBlitFramebuffer(0, 0, width, height, 0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(sGraphicBuffer, 2);
  //glBlitFramebuffer(0, 0, width, height, halfWidth, halfHeight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(sGraphicBuffer, 3);
  //glBlitFramebuffer(0, 0, width, height, halfWidth, 0, width, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
template<typename GraphicBuffer> void GraphicBufferDeferredPassLightEnd(GraphicBuffer const& graphicBuffer)
{
  FrameBufferUnbind(graphicBuffer.mFrameBuffer);
}