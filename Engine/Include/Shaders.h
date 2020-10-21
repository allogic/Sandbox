#pragma once

#include <Core.h>

/*
* Debug shaders.
*/

std::string const sVertexShaderSourceLine
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 2) in vec4 lColor;

uniform mat4 uProjection;
uniform mat4 uView;

out vec3 fPosition;
out vec4 fColor;

void main()
{
  fPosition = lPosition;
  fColor = lColor;

  gl_Position = uProjection * uView * vec4(fPosition, 1.0f);
}
)glsl"
};
std::string const sFragmentShaderSourceLine
{
R"glsl(
#version 460 core

in vec3 fPosition;
in vec4 fColor;

out vec4 color;

void main()
{
  color = fColor;
}
)glsl"
};

/*
* Standard shaders.
*/

std::string const sVertexShaderSourceLambert
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec3 lNormal;
layout (location = 2) in vec4 lColor;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 mTransform;

out vec3 fPosition;
out vec3 fNormal;
out vec4 fColor;

void main()
{
  fPosition = vec3(mTransform * vec4(lPosition, 1.0f));
  fNormal = vec3(mTransform * vec4(lNormal, 1.0f));
  fColor = lColor;

  gl_Position = uProjection * uView * vec4(fPosition, 1.0f);
}
)glsl"
};
std::string const sFragmentShaderSourceLambert
{
R"glsl(
#version 460 core

in vec3 fPosition;
in vec3 fNormal;
in vec4 fColor;

out vec4 color;

void main()
{
  color = vec4(fNormal, 1.0f);
}
)glsl"
};