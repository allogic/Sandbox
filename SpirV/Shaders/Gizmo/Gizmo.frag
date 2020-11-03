#version 460 core

layout (location = 0) in VertOut
{
  vec3 fPosition;
  vec4 fColor;
} fragIn;

layout (location = 0) out vec4 color;

void main()
{
  color = fragIn.fColor;
}