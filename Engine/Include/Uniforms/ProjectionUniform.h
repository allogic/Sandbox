#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global uniform layouts.
*/

struct UniformBlockProjection
{
  r32m4 mProjection     { glm::identity<r32m4>() };
  r32m4 mView           { glm::identity<r32m4>() };
  r32m4 mTransformCamera{ glm::identity<r32m4>() };
  r32m4 mTransformModel { glm::identity<r32m4>() };
};