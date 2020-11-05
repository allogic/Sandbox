#pragma once

#include <Core.h>
#include <Types.h>

/*
* FrameBuffer layouts.
*/

struct FrameBuffer
{
  u32 mFbo{};
};

/*
* FrameBuffer management.
*/

template<typename FrameBuffer> void FrameBufferCreate(FrameBuffer& frameBuffer)
{
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
template<typename FrameBuffer> void FrameBufferUnbind(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
template<typename FrameBuffer> void FrameBufferUnbindRead(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
template<typename FrameBuffer> void FrameBufferUnbindWrite(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
template<typename FrameBuffer> void FrameBufferDestroy(FrameBuffer const& frameBuffer)
{
  glDeleteFramebuffers(1, &frameBuffer.mFbo);
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