#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

/*
* Screen material layouts.
*/

template<typename ShaderLayout>
struct ScreenMaterial
{
  static_assert(ShaderLayout::sLayout == eShaderLayoutScreen);

  ShaderLayout mShaderLayout{};
};

using ScreenMaterialDefault = ScreenMaterial<ShaderScreen>;

/*
* Screen material management.
*/

template<typename ScreenMaterial> void ScreenMaterialCreate(ScreenMaterial& screenMaterial, std::string const& shaderName)
{
  ShaderLayoutCreate(screenMaterial.mShaderLayout, ShaderPaths
  {
    .mVertex  { SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\Screen.vert" },
    .mFragment{ std::string{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Screen\\" } + shaderName + std::string{ ".frag" } },
  });
}
template<typename ScreenMaterial> void ScreenMaterialBind(ScreenMaterial const& screenMaterial)
{
  ShaderLayoutBind(screenMaterial.mShaderLayout);
}
template<typename ScreenMaterial> void ScreenMaterialDestroy(ScreenMaterial const& screenMaterial)
{
  ShaderLayoutDestroy(screenMaterial.mShaderLayout);
}