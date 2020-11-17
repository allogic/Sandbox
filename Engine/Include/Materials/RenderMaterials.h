#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

#include <Uniforms/Projection.h>
#include <Uniforms/Screen.h>
#include <Uniforms/Light.h>

#include <Buffers/Transform.h>

/*
* Render materials.
*
* Bindings
*   Uniforms
*     0 -> ProjectionUniform
*     1 -> SteeringUniform
*     2 -> NoiseUniform
*     x -> AlbedoUniform
*     x -> NormalUniform
*     x -> SpecularUniform
*     x -> RoughnessUniform
*   Buffers
*     0 -> TransformBuffer
*     1 -> SteerigBuffer
*     2 -> PathBuffer
*     3 -> OctreeBuffer
*/

template<typename ShaderLayout>
struct RenderMaterial
{
  static_assert(ShaderLayout::sLayout != eShaderLayoutCompute);

  constexpr static u32 sShaderLayout{ ShaderLayout::sLayout };

  ShaderLayout               mShaderLayout     {};
};

using RenderMaterialLambert          = RenderMaterial<ShaderLambert>;
using RenderMaterialLambertInstanced = RenderMaterial<ShaderLambertInstanced>;
using RenderMaterialGizmo            = RenderMaterial<ShaderGizmo>;

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
}
template<typename RenderMaterial> void RenderMaterialBind(RenderMaterial const& renderMaterial)
{
  ShaderLayoutBind(renderMaterial.mShaderLayout);
}
template<typename RenderMaterial> void RenderMaterialDestroy(RenderMaterial const& renderMaterial)
{
  ShaderLayoutDestroy(renderMaterial.mShaderLayout);
}