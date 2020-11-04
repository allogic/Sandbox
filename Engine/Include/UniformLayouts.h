#pragma once

#include <Core.h>
#include <Types.h>

/*
* Uniform layouts.
*/

template<typename Buffer>
struct UniformLayout
{
  using BufferType = Buffer;

  u32 mUbo{};
};

/*
* Uniform management.
*/

template<typename UniformLayout> void UniformLayoutCreate(UniformLayout& uniformLayout, u32 bufferIndex)
{
  glGenBuffers(1, &uniformLayout.mUbo);

  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
  glBufferStorage(GL_UNIFORM_BUFFER, sizeof(UniformLayout::BufferType), nullptr, GL_DYNAMIC_STORAGE_BIT);

  glBindBufferBase(GL_UNIFORM_BUFFER, bufferIndex, uniformLayout.mUbo);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
template<typename UniformLayout> void UniformLayoutBind(UniformLayout const& uniformLayout)
{
  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
}
template<typename UniformLayout> void UniformLayoutDataSet(UniformLayout const& uniformLayout, u32 bufferSize, void* pBufferData)
{
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize * sizeof(UniformLayout::BufferType), pBufferData);
}
template<typename UniformLayout> void UniformLayoutDestroy(UniformLayout const& uniformLayout)
{
  glDeleteBuffers(1, &uniformLayout.mUbo);
}