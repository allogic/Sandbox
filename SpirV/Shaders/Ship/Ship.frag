#version 460 core

layout (location = 0) in VertOut
{
  vec3 fPosition;
  vec3 fNormal;
  vec2 fUv;
  vec4 fColor;
} fragIn;

layout (location = 0) out vec4 color;

void main()
{
  color = vec4(fragIn.fNormal, 1.f);
}