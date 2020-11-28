#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Texture components.
*/

enum TextureLayoutType : u32
{
  eTextureLayoutU8,
  eTextureLayoutR32,
};
enum TextureFormatType : u32
{
  eTextureFormatRGB,
  eTextureFormatRGBA,
};
enum TextureWrapMode : u32
{
  eTextureWrapRepeat = GL_REPEAT,
  eTextureClampEdge = GL_CLAMP_TO_EDGE,
};
enum TextureFilterMode : u32
{
  eTextureFilterLinear  = GL_LINEAR,
  eTextureFilterNearest = GL_NEAREST,
};

template<u32 Layout, u32 Format>
struct TextureLayout : Component
{
  constexpr static u32 sLayout{ Layout };
  constexpr static u32 sFormat{ Format };

  u32 mWidth {};
  u32 mHeight{};
  u32 mTid   {};
};

using TextureU8RGB   = TextureLayout<eTextureLayoutU8, eTextureFormatRGB>;
using TextureU8RGBA  = TextureLayout<eTextureLayoutU8, eTextureFormatRGB>;
using TextureR32RGB  = TextureLayout<eTextureLayoutR32, eTextureFormatRGB>;
using TextureR32RGBA = TextureLayout<eTextureLayoutR32, eTextureFormatRGBA>;

/*
* Texture management.
*/

template<typename TextureLayout> void TextureLayoutDataSet(TextureLayout const& textureLayout, u32 isDepthTexture, void* pImageData)
{
  s32 formatInternal{};
  s32 format{};
  u32 type{};

  switch (TextureLayout::sLayout)
  {
    case eTextureLayoutU8: type = GL_UNSIGNED_BYTE; break;
    case eTextureLayoutR32: type = GL_FLOAT; break;
  }
  switch (TextureLayout::sFormat)
  {
    case eTextureFormatRGB: formatInternal = GL_RGB8UI; format = GL_RGB; break;
    case eTextureFormatRGBA: formatInternal = GL_RGBA32F; format = GL_RGBA; break;
  }

  if (isDepthTexture)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textureLayout.mWidth, textureLayout.mHeight, 0, GL_DEPTH_COMPONENT, type, pImageData);
  }
  else
  {
    glTexImage2D(GL_TEXTURE_2D, 0, formatInternal, textureLayout.mWidth, textureLayout.mHeight, 0, format, type, pImageData);
  }
}
template<typename TextureLayout> void TextureLayoutCreate(TextureLayout& textureLayout, u32 width, u32 height, u32 isDepthTexture, TextureWrapMode wrapMode = eTextureWrapRepeat, TextureFilterMode filter = eTextureFilterLinear)
{
  textureLayout.mWidth = width;
  textureLayout.mHeight = height;

  glGenTextures(1, &textureLayout.mTid);

  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);

  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_S, (s32)wrapMode);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_T, (s32)wrapMode);

  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MIN_FILTER, (s32)filter);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MAG_FILTER, (s32)filter);

  TextureLayoutDataSet(textureLayout, isDepthTexture, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
}
template<typename TextureLayout> void TextureLayoutBind(TextureLayout const& textureLayout)
{
  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);
}
static                           void TextureLayoutUnbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}
template<typename TextureLayout> void TextureLayoutMapSampler(TextureLayout const& textureLayout, u32 mappingIndex)
{
  glBindTextureUnit(mappingIndex, textureLayout.mTid);
}
template<typename TextureLayout> void TextureLayoutMapTexture(TextureLayout const& textureLayout, u32 mappingIndex)
{
  u32 type{};

  switch (TextureLayout::sLayout)
  {
    case eTextureLayoutU8: type = GL_RGBA8UI; break;
    case eTextureLayoutR32: type = GL_RGBA32F; break;
  }

  glBindImageTexture(mappingIndex, textureLayout.mTid, 0, GL_FALSE, 0, GL_READ_WRITE, type);
}
template<typename TextureLayout> void TextureLayoutDestroy(TextureLayout const& textureLayout)
{
  glDeleteTextures(1, &textureLayout.mTid);
}