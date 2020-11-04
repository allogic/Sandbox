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
template<typename FrameBuffer> void FrameBufferBindRead(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.mFbo);
}
template<typename FrameBuffer> void FrameBufferBindDraw(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.mFbo);
}
template<typename FrameBuffer> void FrameBufferUnbindRead(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
template<typename FrameBuffer> void FrameBufferUnbindDraw(FrameBuffer const& frameBuffer)
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

/*
* FrameBuffer texture attachments.
*/

template<typename TextureLayout> void FrameBufferAttachTexture(TextureLayout const& textureLayout, u32 textureIndex)
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textureIndex, GL_TEXTURE_2D, textureLayout.mTid);
}
template<typename DepthBuffer>   void FrameBufferAttachDepthBuffer(DepthBuffer const& depthBuffer)
{
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.mTid);
}