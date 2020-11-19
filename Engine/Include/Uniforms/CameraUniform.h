#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global uniform layouts.
*/

struct UniformBlockCamera
{
  r32v3 mPosition  {};
  r32v3 mRotation  {};
  r32v3 mLocalRight{};
  r32v3 mLocalUp   {};
  r32v3 mLocalFront{};
};