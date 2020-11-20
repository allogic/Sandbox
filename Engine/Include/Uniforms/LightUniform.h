#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global uniform layouts.
*/

struct alignas(16) UniformBlockPointLight
{
  r32v3 mPosition            {};
  r32   mRadius              {};
  r32v4 mColor               {};
  u32   mEnabled             {};
  r32   mAttenuationLinear   {};
  r32   mAttenuationQuadratic{};
};