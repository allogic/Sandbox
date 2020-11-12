#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

/*
* Compute material layouts.
*/

template<typename ShaderLayout>
struct ComputeMaterial
{
  static_assert(ShaderLayout::sLayout == eShaderLayoutCompute);

  template<typename T>
  static std::vector<UniformLayout<T>> mUniformLayouts{};

  ShaderLayout               mShaderLayout{};

  std::map<std::string, u32> mUniforms    {};
  std::map<std::string, u32> mBuffers     {};
};

using ComputeMaterialDefault = ComputeMaterial<ShaderCompute>;

/*
* Compute material management.
*/

template<typename ComputeMaterial> void ComputeMaterialCreate(ComputeMaterial& computeMaterial, std::string const& shaderName)
{
  ShaderLayoutCreate(computeMaterial.mShaderLayout, ShaderPaths
  {
    .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\" + shaderName + ".vert" }
  });

  ShaderLayoutGetBindingsUniform(computeMaterial.mShaderLayout, computeMaterial.mUniforms);
  ShaderLayoutGetBindingsBuffer(computeMaterial.mShaderLayout, computeMaterial.mBuffers);
}
template<typename ComputeMaterial> void ComputeMaterialBind(ComputeMaterial const& computeMaterial)
{
  ShaderLayoutBind(computeMaterial.mShaderLayout);
}
template<typename ComputeMaterial> void ComputeMaterialCompute(ComputeMaterial const& computeMaterial, u32 numThreadX, u32 numThreadY, u32 numThreadZ)
{
  ShaderLayoutCompute(computeMaterial.mShaderLayout, numThreadX, numThreadY, numThreadZ);
}
template<typename ComputeMaterial> void ComputeMaterialDestroy(ComputeMaterial const& computeMaterial)
{
  ShaderLayoutDestroy(computeMaterial.mShaderLayout);
}