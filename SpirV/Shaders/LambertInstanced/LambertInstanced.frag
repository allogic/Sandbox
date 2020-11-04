#version 460 core

layout (binding = 0) uniform sampler2D uAlbedo;
layout (binding = 1) uniform sampler2D uNormal;
layout (binding = 2) uniform sampler2D uSpecular;
layout (binding = 3) uniform sampler2D uRoughness;

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
  color = vec4(texture(uAlbedo, fragIn.fUv).rgb, 1.f);
}