#pragma once

#include <Core.h>
#include <Types.h>

/*
* Math utlities.
*/

static r32m4 TransformTo(r32v3 const& position, r32v3 const& rotation, r32v3 const& scale)
{
  r32m4 transformation{ glm::identity<r32m4>() };

  transformation = glm::translate(transformation, position);

  transformation = glm::rotate(transformation, rotation.x, { 1.f, 0.f, 0.f });
  transformation = glm::rotate(transformation, rotation.y, { 0.f, 1.f, 0.f });
  transformation = glm::rotate(transformation, rotation.z, { 0.f, 0.f, 1.f });

  transformation = glm::scale(transformation, scale);

  return transformation;
}