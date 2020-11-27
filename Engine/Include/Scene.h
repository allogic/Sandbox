#pragma once

#include <Core.h>
#include <Types.h>

/*
* Scene layouts.
*/

struct Scene
{
  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
  virtual void OnUpdate(r32 timeDelta) = 0;
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnRenderPre() {};
  virtual void OnRender() = 0;
  virtual void OnRenderPost() {};
  virtual void OnGizmos(r32 timeDelta) {}
};