#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global uniform layouts.
*/

struct UniformBlockScreen
{
  r32   mWidth         {};
  r32   mHeight        {};
  r32v3 mCameraPosition{};
};