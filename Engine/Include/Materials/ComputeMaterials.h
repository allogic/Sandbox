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

  ComputeMaterial(u32 numThreadX = 1, u32 numThreadY = 1, u32 numThreadZ = 1)
    : mNumThreadX{ numThreadX }
    , mNumThreadY{ numThreadY }
    , mNumThreadZ{ numThreadZ } {}

  u32          mNumThreadX;
  u32          mNumThreadY;
  u32          mNumThreadZ;

  ShaderLayout mShaderLayout{};
};

using ComputeMaterialDefault = ComputeMaterial<ShaderCompute>;

/*
* Compute material management.
*/

template<typename ComputeMaterial> void ComputeMaterialCreate(ComputeMaterial& computeMaterial, std::string const& shaderName)
{
  ShaderLayoutCreate(computeMaterial.mShaderLayout, ShaderPaths
  {
    .mCompute{ std::string{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\" } + shaderName + std::string{ ".comp" } }
  });
}
template<typename ComputeMaterial> void ComputeMaterialBind(ComputeMaterial const& computeMaterial)
{
  ShaderLayoutBind(computeMaterial.mShaderLayout);
}
template<typename ComputeMaterial> void ComputeMaterialCompute(ComputeMaterial const& computeMaterial)
{
  ShaderLayoutCompute(
    computeMaterial.mShaderLayout,
    computeMaterial.mNumThreadX,
    computeMaterial.mNumThreadY,
    computeMaterial.mNumThreadZ
  );
}
template<typename ComputeMaterial> void ComputeMaterialDestroy(ComputeMaterial const& computeMaterial)
{
  ShaderLayoutDestroy(computeMaterial.mShaderLayout);
}