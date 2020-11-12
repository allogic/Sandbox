#pragma once

#include <Core.h>
#include <Types.h>

/*
* Object layouts.
*/

struct Object
{
  virtual void OnUpdate(r32 timeDelta) {};
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnRender(r32 timeDelta) {};
  virtual void OnGizmos(r32 timeDelta) {}
};