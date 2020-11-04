#pragma once

#include <Core.h>
#include <Types.h>

/*
* Texture layouts.
*/

enum TextureLayoutType : u32
{
  eTextureLayoutU8,
  eTextureLayoutR32,
};

template<u32 Layout, u32 Channels>
struct TextureLayout
{
  constexpr static u32 sLayout  { Layout };
  constexpr static u32 sChannels{ Channels };

  u32 mWidth {};
  u32 mHeight{};
  u32 mTid   {};
};

using TextureU8RGB   = TextureLayout<eTextureLayoutU8, 3>;
using TextureU8RGBA  = TextureLayout<eTextureLayoutU8, 4>;
using TextureR32RGBA = TextureLayout<eTextureLayoutR32, 4>;

/*
* Texture management.
*/

template<typename TextureLayout> void TextureLayoutDataSet(TextureLayout const& textureLayout, void* pImageData)
{
  s32 formatInternal{};
  s32 format{};
  u32 type{};

  switch (TextureLayout::sLayout)
  {
    case eTextureLayoutU8: type = GL_UNSIGNED_BYTE; break;
    case eTextureLayoutR32: type = GL_FLOAT; break;
  }
  switch (TextureLayout::sChannels)
  {
    case 3: formatInternal = GL_RGB8UI; format = GL_RGB; break;
    case 4: formatInternal = GL_RGBA32F; format = GL_RGBA; break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, formatInternal, textureLayout.mWidth, textureLayout.mHeight, 0, format, type, pImageData);
}
template<typename TextureLayout> void TextureLayoutDataSetFrom(TextureLayout const& textureLayout, std::string const& fileName)
{
  s32 width{};
  s32 height{};
  s32 channels{};
  s32 type{};

  switch (TextureLayout::sChannels)
  {
    case 3: type = STBI_rgb; break;
    case 4: type = STBI_rgb_alpha; break;
  }

  u8* pBlob = stbi_load(fileName.data(), &width, &height, &channels, type);

  assert(textureLayout.mWidth == width);
  assert(textureLayout.mHeight == height);
  assert(textureLayout.sChannels == channels);

  u32 textureSize{ textureLayout.mWidth * textureLayout.mHeight * TextureLayout::sChannels };

  if (TextureLayout::sLayout == eTextureLayoutR32)
  {
    std::vector<r32> imageData{};

    imageData.resize(textureSize);

    for (u32 i{}; i < textureSize; i++)
      imageData[i] = pBlob[i] / 255.f;

    TextureLayoutData(textureLayout, imageData.data());
  }
  else
  {
    TextureLayoutData(textureLayout, pBlob);
  }

  stbi_image_free(pBlob);
}
template<typename TextureLayout> void TextureLayoutCreate(TextureLayout& textureLayout, u32 width, u32 height)
{
  textureLayout.mWidth = width;
  textureLayout.mHeight = height;

  glGenTextures(1, &textureLayout.mTid);

  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);

  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  TextureLayoutDataSet(textureLayout, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
}
template<typename TextureLayout> void TextureLayoutBind(TextureLayout const& textureLayout)
{
  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);
}
template<typename TextureLayout> void TextureLayoutBindSampler(TextureLayout const& textureLayout, u32 textureUnit)
{
  glBindTextureUnit(textureUnit, textureLayout.mTid);
}
template<typename TextureLayout> void TextureLayoutBindImage(TextureLayout const& textureLayout, u32 imageUnit)
{
  u32 type{};

  switch (TextureLayout::sLayout)
  {
    case eTextureLayoutU8: type = GL_RGBA8UI; break;
    case eTextureLayoutR32: type = GL_RGBA32F; break;
  }

  glBindImageTexture(imageUnit, textureLayout.mTid, 0, GL_FALSE, 0, GL_READ_WRITE, type);
}
template<typename TextureLayout> void TextureLayoutDestroy(TextureLayout const& textureLayout)
{
  glDeleteTextures(1, &textureLayout.mTid);
}