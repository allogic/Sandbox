#version 460 core

struct LightPoint
{
  vec3 position;
  vec4 color;
  float radius;
};

layout (binding = 0) uniform sampler2D uPosition;
layout (binding = 1) uniform sampler2D uAlbedo;
layout (binding = 2) uniform sampler2D uNormal;
layout (binding = 3) uniform sampler2D uUv;

layout (binding = 0) uniform ProjectionUniform
{
  vec3 uCameraPosition;
  vec2 uScreenSize;
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
  float uTime;
};

layout (binding = 1) uniform LightUniform
{
  LightPoint uPointLights[32];
};

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

layout (location = 0) out vec4 oColor;

void main()
{
  vec3 position = texture(uPosition, fragIn.uv).rgb;
  vec3 albedo = texture(uAlbedo, fragIn.uv).rgb;
  vec3 normal = texture(uNormal, fragIn.uv).rgb;

  vec3 lighting = albedo * 0.1f;

  vec3 viewDir = normalize(uCameraPosition - position);

  //for (uint i = 0; i < 32; i++)
  //{
  //  vec3 lightDir = normalize(uPointLights[i].position - position);
  //  vec3 diffuse = max(dot(normal, lightDir), 0.f) * albedo * uPointLights[i].color.rgb;
  //
  //  lighting += diffuse;
  //}

  oColor = vec4(lighting, 1.f);
}