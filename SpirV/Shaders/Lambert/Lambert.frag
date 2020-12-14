#version 460 core

layout (binding = 0) uniform sampler2D uAlbedo;
layout (binding = 1) uniform sampler2D uNormal;
layout (binding = 2) uniform sampler2D uSpecular;
layout (binding = 3) uniform sampler2D uHeight;
layout (binding = 4) uniform sampler2D uMetallic;
layout (binding = 5) uniform sampler2D uRoughness;

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
layout (location = 3) out vec4 oSpecular;
layout (location = 4) out vec4 oHeight;
layout (location = 5) out vec4 oMetallic;
layout (location = 6) out vec4 oRoughness;

void main()
{
  oPosition = vec4(fragIn.position, 1.f);
  oAlbedo = texture(uAlbedo, fragIn.uv);
  oNormal = texture(uNormal, fragIn.uv);
  oSpecular = texture(uSpecular, fragIn.uv);
  oHeight = texture(uHeight, fragIn.uv);
  oMetallic = texture(uMetallic, fragIn.uv);
  oRoughness = texture(uRoughness, fragIn.uv);
}