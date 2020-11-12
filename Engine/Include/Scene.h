#pragma once

#include <Core.h>
#include <Types.h>
#include <Camera.h>
#include <Registry.h>

#include <Layouts/UniformLayouts.h>

#include <Uniforms/Projection.h>

#include <GraphicBuffer.h>

/*
* Scene layouts.
*/

struct Scene
{
  Scene(u32 width, u32 height);
  virtual ~Scene();

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
  virtual void OnUpdate(r32 timeDelta) = 0;
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnRender(r32 timeDelta) = 0;
  virtual void OnGizmos(r32 timeDelta) {}

  Camera                     mCamera{};

  GraphicBuffer&             mGraphicBuffer    { RegistryGetOrCreate<GraphicBuffer>("graphicBuffer") };
  UniformLayout<Projection>& mUniformProjection{ RegistryGetOrCreate<UniformLayout<Projection>>("uniformProjection") };
};