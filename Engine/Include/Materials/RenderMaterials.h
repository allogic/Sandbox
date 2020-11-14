#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

#include <Uniforms/Projection.h>
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

  UniformLayout<Projection>& mUniformProjection{ RegistryGetOrCreate<UniformLayout<Projection>>("uniformProjection") };
  BufferLayout<Transform>&   mBufferTransform  { RegistryGetOrCreate<BufferLayout<Transform>>("bufferTransform") };
};

using RenderMaterialLambert          = RenderMaterial<ShaderLambert>;
using RenderMaterialLambertInstanced = RenderMaterial<ShaderLambertInstanced>;
using RenderMaterialGizmo            = RenderMaterial<ShaderGizmo>;

/*
* Render material management.
*/

template<typename RenderMaterial, typename ... Args> void RenderMaterialCreate(RenderMaterial& renderMaterial, Args&& ... args)
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
      BufferLayoutCreate(renderMaterial.mBufferTransform, std::forward<Args>(args) ...);
      BufferLayoutBind(renderMaterial.mBufferTransform);
      BufferLayoutMap(renderMaterial.mBufferTransform, 0);

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
template<typename RenderMaterial>                    void RenderMaterialBind(RenderMaterial const& renderMaterial)
{
  ShaderLayoutBind(renderMaterial.mShaderLayout);

  UniformLayoutBind(renderMaterial.mUniformProjection);
  UniformLayoutMap(renderMaterial.mUniformProjection, 0);

  switch (RenderMaterial::sShaderLayout)
  {
    case eShaderLayoutLambert:
    {
      break;
    }
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
template<typename RenderMaterial>                    void RenderMaterialDestroy(RenderMaterial const& renderMaterial)
{
  ShaderLayoutDestroy(renderMaterial.mShaderLayout);
}