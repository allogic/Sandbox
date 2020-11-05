#pragma once

#include <Core.h>
#include <Types.h>

#include <FrameBuffer.h>
#include <DepthBuffer.h>

#include <TextureLayouts.h>

/*
* GraphicBuffer layouts.
*/

struct GraphicBuffer
{
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
  FrameBufferCreate(graphicBuffer.mFrameBuffer);
  DepthBufferCreate(graphicBuffer.mDepthBuffer, width, height);

  TextureLayoutCreate(graphicBuffer.mTexturePosition, width, height);
  TextureLayoutCreate(graphicBuffer.mTextureAlbedo, width, height);
  TextureLayoutCreate(graphicBuffer.mTextureNormal, width, height);
  TextureLayoutCreate(graphicBuffer.mTextureUv, width, height);

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