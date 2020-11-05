#version 460 core

layout (location = 0) in VertOut
{
  vec3 position;
  vec4 color;
} fragIn;

layout (location = 0) out vec4 oColor;

void main()
{
  oColor = fragIn.color;
}