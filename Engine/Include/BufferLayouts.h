#pragma once

#include <Core.h>
#include <Types.h>

/*
* SSBO layouts.
*/

template<typename Storage>
struct BufferLayout
{
  using StorageType = Storage;

  u32 mSsbo{};
};

/*
* SSBO management.
*/

template<typename BufferLayout>                   void BufferLayoutCreate(BufferLayout& bufferLayout, u32 storageBufferSize, u32 storageBufferIndex)
{
  glGenBuffers(1, &bufferLayout.mSsbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * sizeof(BufferLayout::StorageType), nullptr, GL_STATIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storageBufferIndex, bufferLayout.mSsbo);
}
template<typename BufferLayout>                   void BufferLayoutClear(BufferLayout const& bufferLayout, u32 storageBufferSize)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * sizeof(BufferLayout::StorageType), nullptr, GL_STATIC_DRAW);
}
template<typename BufferLayout, typename Storage> void BufferLayoutData(BufferLayout const& bufferLayout, Storage* pStorageData, u32 storageBufferSize)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * sizeof(BufferLayout::StorageType), pStorageData, GL_STATIC_DRAW);
}
template<typename BufferLayout, typename Storage> void BufferLayoutDataSubGet(BufferLayout const& bufferLayout, u32 storageBufferOffset, u32 storageBufferSize, Storage* pStorageData)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, storageBufferOffset * sizeof(BufferLayout::StorageType), storageBufferSize * sizeof(BufferLayout::StorageType), pStorageData);
}
template<typename BufferLayout>                   void BufferLayoutDestroy(BufferLayout const& bufferLayout)
{
  glDeleteBuffers(1, &bufferLayout.mSsbo);
}