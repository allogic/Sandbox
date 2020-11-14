#version 460 core

layout (binding = 0) uniform sampler2D uAlbedo;
layout (binding = 1) uniform sampler2D uNormal;
layout (binding = 2) uniform sampler2D uSpecular;
layout (binding = 3) uniform sampler2D uRoughness;

layout (location = 0) in VertOut
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} fragIn;

layout (location = 0) out vec3 oPosition;
layout (location = 1) out vec3 oAlbedo;
layout (location = 2) out vec3 oNormal;
layout (location = 3) out vec3 oUv;

void main()
{
  oPosition = fragIn.position;
  oAlbedo = vec3(1, 0, 0); //texture(uAlbedo, fragIn.uv).rgb;
  oNormal = fragIn.normal;
  oUv = vec3(fragIn.uv, 0.f);
}