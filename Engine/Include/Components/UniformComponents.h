#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Uniform components.
*/

template<typename Buffer>
struct UniformLayout : Component
{
  using BufferType = Buffer;

  u32 mUbo{};
};

/*
* Uniform management.
*/

template<typename UniformLayout> void UniformLayoutCreate(UniformLayout& uniformLayout, u32 bufferSize = 1)
{
  glGenBuffers(1, &uniformLayout.mUbo);

  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
  glBufferStorage(GL_UNIFORM_BUFFER, bufferSize * sizeof(UniformLayout::BufferType), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
template<typename UniformLayout> void UniformLayoutBind(UniformLayout const& uniformLayout)
{
  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
}
static                           void UniformLayoutUnbind()
{
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
template<typename UniformLayout> void UniformLayoutMap(UniformLayout const& uniformLayout, u32 mappingIndex)
{
  glBindBufferBase(GL_UNIFORM_BUFFER, mappingIndex, uniformLayout.mUbo);
}
template<typename UniformLayout> void UniformLayoutDataSet(UniformLayout const& uniformLayout, u32 bufferSize, void* pBufferData)
{
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize * sizeof(UniformLayout::BufferType), pBufferData);
}
template<typename UniformLayout> void UniformLayoutDataGet(UniformLayout const& uniformLayout, u32 bufferSize, void* pBufferData)
{
  glGetBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize * sizeof(UniformLayout::BufferType), pBufferData);
}
template<typename UniformLayout> void UniformLayoutDestroy(UniformLayout const& uniformLayout)
{
  glDeleteBuffers(1, &uniformLayout.mUbo);
}