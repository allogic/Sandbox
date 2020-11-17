#pragma once

#include <Core.h>
#include <Types.h>

/*
* Light layouts.
*/

struct LightPoint
{
  r32v3 mPosition{};
  r32v4 mColor   {};
  r32   mRadius  {};
};