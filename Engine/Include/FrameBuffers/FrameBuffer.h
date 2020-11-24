#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/TextureLayouts.h>

/*
* FrameBuffer layouts.
*/

struct FrameBufferDeferred
{
  u32             mWidth          {};
  u32             mHeight         {};
  u32             mFbo            {};
  TextureR32RGBA  mTexturePosition{};
  TextureR32RGBA  mTextureAlbedo  {};
  TextureR32RGBA  mTextureNormal  {};
  TextureR32RGBA  mTextureUv      {};
  TextureR32Depth mTextureDepth   {};
};

/*
* FrameBuffer management.
*/

template<typename FrameBuffer> void FrameBufferCreate(FrameBuffer& frameBuffer, u32 width, u32 height)
{
  frameBuffer.mWidth = width;
  frameBuffer.mHeight = height;

  glGenFramebuffers(1, &frameBuffer.mFbo);

  TextureLayoutCreate(frameBuffer.mTexturePosition, frameBuffer.mWidth, frameBuffer.mHeight, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureAlbedo, frameBuffer.mWidth, frameBuffer.mHeight, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureNormal, frameBuffer.mWidth, frameBuffer.mHeight, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureUv, frameBuffer.mWidth, frameBuffer.mHeight, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureDepth, frameBuffer.mWidth, frameBuffer.mHeight, eTextureClampEdge, eTextureFilterNearest);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.mFbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.mTexturePosition.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, frameBuffer.mTextureAlbedo.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, frameBuffer.mTextureNormal.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, frameBuffer.mTextureUv.mTid, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.mTextureDepth.mTid, 0);

  u32 buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

  glDrawBuffers(4, buffers);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
static                         void FrameBufferSetReadTexture(u32 textureIndex)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0 + textureIndex);
}
template<typename FrameBuffer> void FrameBufferDestroy(FrameBuffer const& frameBuffer)
{
  glDeleteFramebuffers(1, &frameBuffer.mFbo);

  TextureLayoutDestroy(frameBuffer.mTexturePosition);
  TextureLayoutDestroy(frameBuffer.mTextureAlbedo);
  TextureLayoutDestroy(frameBuffer.mTextureNormal);
  TextureLayoutDestroy(frameBuffer.mTextureUv);
  TextureLayoutDestroy(frameBuffer.mTextureDepth);
}