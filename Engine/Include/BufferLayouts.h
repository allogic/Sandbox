#pragma once

#include <Core.h>
#include <Types.h>

/*
* SSBO layouts.
*/

template<typename Buffer>
struct BufferLayout
{
  using BufferType = Buffer;

  u32 mSsbo{};
};

/*
* SSBO management.
*/

template<typename BufferLayout> void BufferLayoutCreate(BufferLayout& bufferLayout, u32 bufferSize, u32 bufferIndex)
{
  glGenBuffers(1, &bufferLayout.mSsbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferSize * sizeof(BufferLayout::BufferType), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferIndex, bufferLayout.mSsbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template<typename BufferLayout> void BufferLayoutBind(BufferLayout const& bufferLayout)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
}
template<typename BufferLayout> void BufferLayoutDataSet(BufferLayout const& bufferLayout, u32 bufferSize, void* pBufferData)
{
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(BufferLayout::BufferType), pBufferData);
}
template<typename BufferLayout> void BufferLayoutDataGet(BufferLayout const& bufferLayout, u32 bufferSize, void* pBufferData)
{
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(BufferLayout::BufferType), pBufferData);
}
template<typename BufferLayout> void BufferLayoutDestroy(BufferLayout const& bufferLayout)
{
  glDeleteBuffers(1, &bufferLayout.mSsbo);
}