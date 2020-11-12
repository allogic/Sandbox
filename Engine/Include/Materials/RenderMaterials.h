#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

#include <Uniforms/Projection.h>

/*
* Render materials.
*/

template<typename ShaderLayout>
struct RenderMaterial
{
  static_assert(ShaderLayout::sLayout != eShaderLayoutCompute);

  constexpr static u32 sShaderLayout{ ShaderLayout::sLayout };

  ShaderLayout               mShaderLayout     {};

  std::map<std::string, u32> mUniforms         {};
  std::map<std::string, u32> mBuffers          {};

  UniformLayout<Projection>& mUniformProjection{ RegistryGetOrCreate<UniformLayout<Projection>>("uniformProjection") };
};

using RenderMaterialLambert          = RenderMaterial<ShaderLambert>;
using RenderMaterialLambertInstanced = RenderMaterial<ShaderLambertInstanced>;
using RenderMaterialGizmo            = RenderMaterial<ShaderGizmo>;

/*
* Render material utilities.
*/

template<typename RenderMaterial> u32 RenderMaterialGetUniformIndex(RenderMaterial const& renderMaterial, std::string const& uniformName)
{
  auto const uniformIt{ renderMaterial.mUniforms.find(uniformName) };
  assert(uniformIt != renderMaterial.mUniforms.end());
  return uniformIt->second;
}

/*
* Render material management.
*/

template<typename RenderMaterial> void RenderMaterialCreate(RenderMaterial& renderMaterial)
{
  switch (RenderMaterial::sShaderLayout)
  {
    case eShaderLayoutLambert:
    {
      ShaderLayoutCreate(renderMaterial.mShaderLayout, ShaderPaths
      {
        .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" },
        .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" }
      });
      break;
    }
    case eShaderLayoutLambertInstanced:
    {
      ShaderLayoutCreate(renderMaterial.mShaderLayout, ShaderPaths
      {
        .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\LambertInstanced\\LambertInstanced.vert" },
        .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\LambertInstanced\\LambertInstanced.frag" },
      });
      break;
    }
    case eShaderLayoutGizmo:
    {
      ShaderLayoutCreate(renderMaterial.mShaderLayout, ShaderPaths
      {
        .mVertex{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.vert" },
        .mFragment{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.frag" }
      });
      break;
    }
  }

  ShaderLayoutGetBindingsUniform(renderMaterial.mShaderLayout, renderMaterial.mUniforms);
  ShaderLayoutGetBindingsBuffer(renderMaterial.mShaderLayout, renderMaterial.mBuffers);
}
template<typename RenderMaterial> void RenderMaterialBind(RenderMaterial const& renderMaterial)
{
  ShaderLayoutBind(renderMaterial.mShaderLayout);

  UniformLayoutBind(renderMaterial.mUniformProjection);
  u32 projectionIndex{ RenderMaterialGetUniformIndex(renderMaterial, "ProjectionUniform") };
  UniformLayoutMap(renderMaterial.mUniformProjection, projectionIndex);

  switch (RenderMaterial::sShaderLayout)
  {
    case eShaderLayoutLambert:
    case eShaderLayoutLambertInstanced:
    {


      break;
    }
    case eShaderLayoutGizmo:
    {

      break;
    }
  }
}
template<typename RenderMaterial> void RenderMaterialDestroy(RenderMaterial const& renderMaterial)
{
  ShaderLayoutDestroy(renderMaterial.mShaderLayout);
}