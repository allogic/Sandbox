#pragma once

#include <Core.h>
#include <Types.h>

/*
* DepthBuffer layouts.
*/

struct DepthBuffer
{
  u32 mTid{};
};

/*
* DepthBuffer management.
*/

template<typename DepthBuffer> void DepthBufferCreate(DepthBuffer& depthBuffer, u32 width, u32 height)
{
  glGenTextures(1, &depthBuffer.mTid);

  glBindTexture(GL_TEXTURE_2D, depthBuffer.mTid);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
}
template<typename DepthBuffer> void DepthBufferBind(DepthBuffer const& depthBuffer)
{
  glBindTexture(depthBuffer.mTid);
}