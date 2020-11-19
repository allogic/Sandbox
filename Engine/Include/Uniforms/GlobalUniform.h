#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global uniform layouts.
*/

struct UniformBlockGlobal
{
  r32   mTime      {};
  r32   mTimeDelta {};
  r32v2 mScreenSize{};
};