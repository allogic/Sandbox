#pragma once

#include <Core.h>
#include <Types.h>

#include <Camera.h>

/*
* Scene layouts.
*/

struct Scene
{
  Camera mCamera{};

  virtual ~Scene() = default;

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
  virtual void OnUpdate(r32 timeDelta) = 0;
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnRender(r32 timeDelta) = 0;
  virtual void OnGizmos(r32 timeDelta) {}
};