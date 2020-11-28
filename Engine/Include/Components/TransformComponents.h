#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Transform components.
*/

struct Transform : Component
{
  r32v3 mPosition{};
  r32v3 mRotation{};
  r32v3 mScale   { 1.f, 1.f, 1.f };
};