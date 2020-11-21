#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global buffer layouts.
*/

struct BufferTransform
{
  r32v3 mPosition          {};
  r32v3 mRotation          {};
  r32v3 mRotationLocalRight{};
  r32v3 mRotationLocalUp   {};
  r32v3 mRotationLocalFront{};
};