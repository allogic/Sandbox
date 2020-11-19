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

enum TextureChannelType : u32
{
  eRGB,
  eRGBA,
  eDepth,
};

template<u32 Layout, u32 Channel>
struct TextureLayout
{
  constexpr static TextureLayoutType  sLayout { Layout };
  constexpr static TextureChannelType sChannel{ Channel };

  u32 mWidth {};
  u32 mHeight{};
  u32 mTid   {};
};

using TextureU8RGB   = TextureLayout<eTextureLayoutU8, eRGB>;
using TextureU8RGBA  = TextureLayout<eTextureLayoutU8, eRGB>;
using TextureR32RGB = TextureLayout<eTextureLayoutR32, eRGB>;
using TextureR32RGBA = TextureLayout<eTextureLayoutR32, eRGBA>;

using DepthR32RGBA   = TextureLayout<eTextureLayoutR32, eDepth>;

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
  switch (TextureLayout::sChannel)
  {
    case eRGB: formatInternal = GL_RGB8UI; format = GL_RGB; break;
    case eRGBA: formatInternal = GL_RGBA32F; format = GL_RGBA; break;
    case eDepth: formatInternal = GL_DEPTH_COMPONENT32F; format = GL_DEPTH_COMPONENT; break;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, formatInternal, textureLayout.mWidth, textureLayout.mHeight, 0, format, type, pImageData);
}
template<typename TextureLayout> void TextureLayoutDataSetFrom(TextureLayout const& textureLayout, std::string const& fileName)
{
  s32 width{};
  s32 height{};
  s32 channels{};
  s32 type{};

  switch (TextureLayout::sChannel)
  {
    case eRGB: type = STBI_rgb; break;
    case eRGBA: type = STBI_rgb_alpha; break;
  }

  u8* pBlob = stbi_load(fileName.data(), &width, &height, &channels, type);

  assert(textureLayout.mWidth == width);
  assert(textureLayout.mHeight == height);

  switch (TextureLayout::sChannel)
  {
    case eRGB: assert(3 == channels); break;
    case eRGBA: assert(4 == channels); break;
  }

  u32 textureSize{};

  switch (TextureLayout::sChannel)
  {
    case eRGB: textureSize = textureLayout.mWidth * textureLayout.mHeight * 3; break;
    case eRGBA: textureSize = textureLayout.mWidth * textureLayout.mHeight * 4; break;
  }

  if (TextureLayout::sLayout == eTextureLayoutR32)
  {
    std::vector<r32> imageData{};

    imageData.resize(textureSize);

    for (u32 i{}; i < textureSize; i++)
      imageData[i] = pBlob[i] / 255.f;

    TextureLayoutDataSet(textureLayout, imageData.data());
  }
  else
  {
    TextureLayoutDataSet(textureLayout, pBlob);
  }

  stbi_image_free(pBlob);
}
template<typename TextureLayout> void TextureLayoutCreate(TextureLayout& textureLayout, u32 width, u32 height, u32 wrapMode = GL_REPEAT, u32 filter = GL_LINEAR)
{
  textureLayout.mWidth = width;
  textureLayout.mHeight = height;

  glGenTextures(1, &textureLayout.mTid);

  glBindTexture(GL_TEXTURE_2D, textureLayout.mTid);

  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_S, wrapMode);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_WRAP_T, wrapMode);

  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MIN_FILTER, filter);
  glTextureParameteri(textureLayout.mTid, GL_TEXTURE_MAG_FILTER, filter);

  TextureLayoutDataSet(textureLayout, nullptr);

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