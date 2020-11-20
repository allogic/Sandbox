#pragma once

#include <Core.h>
#include <Types.h>

/*
* Math utlities.
*/

static r32m4 TransformTo(r32v3 const& position, r32v3 const& rotation, r32v3 const& scale)
{
  r32m4 transform{ glm::identity<r32m4>() };

  transform = glm::translate(transform, position);

  transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
  transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
  transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });

  transform = glm::scale(transform, scale);

  return transform;
}