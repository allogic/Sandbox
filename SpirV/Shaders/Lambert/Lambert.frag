#version 460 core

layout (binding = 1) uniform sampler2D uDiffuse;

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
  color = vec4(texture(uDiffuse, fragIn.fUv).rgb, 1.f);
}