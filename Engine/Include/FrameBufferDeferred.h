#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/TextureLayouts.h>

/*
* FrameBuffer layouts.
*/

struct FrameBufferDeferred
{
  u32            mWidth           {};
  u32            mHeight          {};
  u32            mFbo             {};
  TextureR32RGBA mTexturePosition {};
  TextureR32RGBA mTextureAlbedo   {};
  TextureR32RGBA mTextureNormal   {};
  TextureR32RGBA mTextureSpecular {};
  TextureR32RGBA mTextureHeight   {};
  TextureR32RGBA mTextureMetallic {};
  TextureR32RGBA mTextureRoughness{};
  TextureR32RGBA mTextureDepth    {};
};

/*
* FrameBuffer management.
*/

template<typename FrameBuffer> void FrameBufferCreate(FrameBuffer& frameBuffer, u32 width, u32 height)
{
  frameBuffer.mWidth = width;
  frameBuffer.mHeight = height;

  glGenFramebuffers(1, &frameBuffer.mFbo);

  TextureLayoutCreate(frameBuffer.mTexturePosition, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureAlbedo, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureNormal, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureSpecular, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureHeight, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureMetallic, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureRoughness, frameBuffer.mWidth, frameBuffer.mHeight, 0, eTextureClampEdge, eTextureFilterNearest);
  TextureLayoutCreate(frameBuffer.mTextureDepth, frameBuffer.mWidth, frameBuffer.mHeight, 1, eTextureClampEdge, eTextureFilterNearest);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.mFbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.mTexturePosition.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, frameBuffer.mTextureAlbedo.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, frameBuffer.mTextureNormal.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, frameBuffer.mTextureSpecular.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, frameBuffer.mTextureHeight.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, frameBuffer.mTextureMetallic.mTid, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, frameBuffer.mTextureRoughness.mTid, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.mTextureDepth.mTid, 0);

  u32 buffers[]
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
  };

  glDrawBuffers(7, buffers);

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
  TextureLayoutDestroy(frameBuffer.mTextureSpecular);
  TextureLayoutDestroy(frameBuffer.mTextureHeight);
  TextureLayoutDestroy(frameBuffer.mTextureMetallic);
  TextureLayoutDestroy(frameBuffer.mTextureRoughness);
  TextureLayoutDestroy(frameBuffer.mTextureDepth);
}