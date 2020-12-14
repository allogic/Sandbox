#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Light components.
*/

struct LightPoint : Component
{
  r32   mRadius              {};
  r32v4 mColor               {};
  r32   mAttenuationLinear   {};
  r32   mAttenuationQuadratic{};
};