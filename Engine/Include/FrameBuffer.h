#pragma once

#include <Core.h>
#include <Types.h>

/*
* FrameBuffer layouts.
*/

struct FrameBuffer
{
  u32 mWidth {};
  u32 mHeight{};
  u32 mFbo   {};
};

/*
* FrameBuffer management.
*/

template<typename FrameBuffer> void FrameBufferCreate(FrameBuffer& frameBuffer, u32 width, u32 height)
{
  frameBuffer.mWidth = width;
  frameBuffer.mHeight = height;

  glGenFramebuffers(1, &frameBuffer.mFbo);
}
template<typename FrameBuffer> void FrameBufferBind(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.mFbo);
}
template<typename FrameBuffer> void FrameBufferBindRead(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.mFbo);
}
template<typename FrameBuffer> void FrameBufferBindWrite(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.mFbo);
}
static                         void FrameBufferUnbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
static                         void FrameBufferUnbindRead()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
static                         void FrameBufferUnbindWrite()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
static                         void FrameBufferReadBuffer(u32 textureIndex)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
}
template<typename FrameBuffer> void FrameBufferDestroy(FrameBuffer const& frameBuffer)
{
  glDeleteFramebuffers(1, &frameBuffer.mFbo);
}

/*
* Deferred render passes.
*/

template<typename FrameBuffer> void FrameBufferDeferredPassGeometryBegin(FrameBuffer const& frameBuffer)
{
  FrameBufferBindWrite(frameBuffer);

  glDepthMask(GL_TRUE);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_BLEND);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
template<typename FrameBuffer> void FrameBufferDeferredPassGeometryEnd(FrameBuffer const& frameBuffer)
{
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  FrameBufferUnbind();
}
template<typename FrameBuffer> void FrameBufferDeferredPassLightBegin(FrameBuffer const& frameBuffer)
{
  FrameBufferBindRead(frameBuffer);

  //glClearColor(0.f, 0.f, 0.f, 1.f);
  //glClear(GL_COLOR_BUFFER_BIT);

  u32 width{ frameBuffer.mWidth };
  u32 height{ frameBuffer.mHeight };
  u32 halfWidth{ (u32)(width / 2.f) };
  u32 halfHeight{ (u32)(height / 2.f) };

  FrameBufferReadBuffer(1);
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  //GraphicBufferReadBuffer(graphicBuffer, 0);
  //glBlitFramebuffer(0, 0, width, height, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(graphicBuffer, 1);
  //glBlitFramebuffer(0, 0, width, height, 0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(graphicBuffer, 2);
  //glBlitFramebuffer(0, 0, width, height, halfWidth, halfHeight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  //
  //GraphicBufferReadBuffer(graphicBuffer, 3);
  //glBlitFramebuffer(0, 0, width, height, halfWidth, 0, width, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
template<typename FrameBuffer> void FrameBufferDeferredPassLightEnd(FrameBuffer const& frameBuffer)
{
  FrameBufferUnbind();
}

/*
* FrameBuffer texture attachments.
*/

template<typename TextureLayout> void FrameBufferAttachTexture(TextureLayout const& textureLayout, u32 textureIndex)
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textureIndex, GL_TEXTURE_2D, textureLayout.mTid, 0);
}
template<typename DepthBuffer>   void FrameBufferAttachDepthBuffer(DepthBuffer const& depthBuffer)
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.mTid, 0);
}