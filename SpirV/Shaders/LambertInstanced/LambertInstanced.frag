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

layout (location = 0) out vec4 oPosition;
layout (location = 1) out vec4 oAlbedo;
layout (location = 2) out vec4 oNormal;
layout (location = 3) out vec4 oUv;

void main()
{
  oPosition = vec4(fragIn.position, 1.f);
  oAlbedo = texture(uAlbedo, fragIn.uv);
  oNormal = vec4(fragIn.normal, 1.f);
  oUv = vec4(fragIn.uv, 0.f, 1.f);
}