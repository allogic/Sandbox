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
  TextureR32RGBA mTextureDiffuse {};
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
  TextureLayoutCreate(graphicBuffer.mTextureDiffuse, width, height);
  TextureLayoutCreate(graphicBuffer.mTextureNormal, width, height);
  TextureLayoutCreate(graphicBuffer.mTextureUv, width, height);

  FrameBufferBindDraw(graphicBuffer.mFrameBuffer);

  FrameBufferAttachTexture(graphicBuffer.mTexturePosition, 0);
  FrameBufferAttachTexture(graphicBuffer.mTexturePosition, 1);
  FrameBufferAttachTexture(graphicBuffer.mTexturePosition, 2);
  FrameBufferAttachTexture(graphicBuffer.mTexturePosition, 3);

  FrameBufferAttachDepthBuffer(graphicBuffer.mDepthBuffer);

  u32 buffers[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

  glDrawBuffers(4, buffers);

  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (Status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", Status);
    return false;
  }

  FrameBufferUnbindDraw(graphicBuffer.mFrameBuffer);
}
template<typename GraphicBuffer> void GraphicBufferBind(GraphicBuffer const& graphicBuffer)
{

}