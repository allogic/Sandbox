#pragma once

#include <Core.h>

std::string const sLineVertexShaderSource
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec3 lNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 mTransform;

out vec3 position;
out vec3 normal;

void main()
{
  position = vec3(mTransform * vec4(lPosition, 1.0f));
  normal = vec3(mTransform * vec4(lNormal, 1.0f));

  gl_Position = uProjection * uView * vec4(position, 1.0f);
}
)glsl"
};
std::string const sLineFragmentShaderSource
{
R"glsl(
#version 460 core

in vec3 position;
in vec3 normal;

out vec4 color;

void main()
{
  color = vec4(normal, 1.0f);
}
)glsl"
};