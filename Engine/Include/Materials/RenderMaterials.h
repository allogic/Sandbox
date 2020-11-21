#pragma once

#include <Core.h>
#include <Types.h>

#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>
#include <Layouts/BufferLayouts.h>

/*
* Render material layouts.
*/

template<typename ShaderLayout>
struct RenderMaterial
{
  static_assert
  (
    ShaderLayout::sLayout != eShaderLayoutCompute &&
    ShaderLayout::sLayout != eShaderLayoutScreen
  );

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
        .mVertex{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.vert" },
        .mFragment{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" }
      });
      break;
    }
    case eShaderLayoutLambertInstanced:
    {
      ShaderLayoutCreate(renderMaterial.mShaderLayout, ShaderPaths
      {
        .mVertex{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Lambert\\LambertInstanced.vert" },
        .mFragment{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Lambert\\Lambert.frag" },
      });
      break;
    }
    case eShaderLayoutGizmo:
    {
      ShaderLayoutCreate(renderMaterial.mShaderLayout, ShaderPaths
      {
        .mVertex{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.vert" },
        .mFragment{ SANDBOX_ENGINE_ROOT_PATH "SpirV\\Compiled\\Gizmo\\Gizmo.frag" }
      });
      break;
    }
  }
}
template<typename RenderMaterial> void RenderMaterialBind(RenderMaterial const& renderMaterial)
{
  ShaderLayoutBind(renderMaterial.mShaderLayout);
}
static                            void RenderMaterialUnbind()
{
  ShaderLayoutUnbind();
}
template<typename RenderMaterial> void RenderMaterialDestroy(RenderMaterial const& renderMaterial)
{
  ShaderLayoutDestroy(renderMaterial.mShaderLayout);
}