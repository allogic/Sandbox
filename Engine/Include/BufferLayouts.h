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

  constexpr static u32 sStorageSizeBytes{ sizeof(Storage) };

  u32 mSsbo{};
};

/*
* SSBO management.
*/

template<typename BufferLayout>                   void BufferLayoutCreate(BufferLayout& bufferLayout, u32 storageBufferSize)
{
  glGenBuffers(1, &bufferLayout.mSsbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * BufferLayout::sStorageSizeBytes, nullptr, GL_STATIC_COPY);
}
template<typename BufferLayout>                   void BufferLayoutBind(BufferLayout const& bufferLayout, u32 storageBufferIndex)
{
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storageBufferIndex, bufferLayout.mSsbo);
}
template<typename BufferLayout>                   void BufferLayoutClear(BufferLayout const& bufferLayout, u32 storageBufferSize)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * BufferLayout::sStorageSizeBytes, nullptr, GL_STATIC_COPY);
}
template<typename BufferLayout, typename Storage> void BufferLayoutData(BufferLayout const& bufferLayout, Storage* pStorageData, u32 storageBufferSize)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferData(GL_SHADER_STORAGE_BUFFER, storageBufferSize * BufferLayout::sStorageSizeBytes, pStorageData, GL_STATIC_COPY);
}
template<typename BufferLayout, typename Storage> void BufferLayoutDataSub(BufferLayout const& bufferLayout, Storage* pStorageData, u32 storageBufferOffset, u32 storageBufferSize)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, storageBufferOffset * BufferLayout::sStorageSizeBytes, storageBufferSize * BufferLayout::sStorageSizeBytes, pStorageData);
}
template<typename BufferLayout>                   void BufferLayoutDestroy(BufferLayout const& bufferLayout)
{
  glDeleteBuffers(1, &bufferLayout.mSsbo);
}