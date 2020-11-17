#pragma once

#include <Core.h>
#include <Types.h>
#include <Lights.h>

/*
* Global uniform layouts.
*/

struct UniformBlockPointLight
{
  LightPoint lights[32];
};