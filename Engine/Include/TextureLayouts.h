#pragma once

#include <Core.h>
#include <Types.h>

/*
* Texture layouts.
*/

template<typename Value, u32 Channels>
struct TextureLayout
{
  using ValueType = Value;

  constexpr static u32 sChannels{ Channels };

  u32 mWidth {};
  u32 mHeight{};
  u32 mTid   {};
};

using TextureU8RGB  = TextureLayout<u8, 3>;
using TextureU8RGBA = TextureLayout<u8, 4>;

/*
* Texture management.
*/

template<typename TextureLayout>                 void TextureLayoutCreate(TextureLayout& textureLayout, u32 width, u32 height)
{
  textureLayout.mWidth = width;
  textureLayout.mHeight = height;

  glGenTextures(1, &textureLayout.mTid);

  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);

  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glBindTexture(GL_TEXTURE_2D, 0);
}
template<typename TextureLayout, typename Value> void TextureLayoutData(TextureLayout& textureLayout, Value* pImageData)
{
  s32 format{};
  u32 type{};

  switch (TextureLayout::sChannels)
  {
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
  }
  switch (sizeof(TextureLayout::ValueType))
  {
    case sizeof(u8): type = GL_UNSIGNED_BYTE; break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, textureLayout.mWidth, textureLayout.mHeight, 0, format, type, pImageData);
}
template<typename TextureLayout>                 void TextureLayoutDestroy(TextureLayout const& textureLayout)
{
  glDeleteTextures(1, &textureLayout.mTid);
}