#version 460 core

struct LightPoint
{
  vec3 position;
  float radius;
  vec4 color;
  uint enabled;
  float attenuationLinear;
  float attenuationQuadratic;
};

layout (binding = 0) uniform sampler2D uPosition;
layout (binding = 1) uniform sampler2D uAlbedo;
layout (binding = 2) uniform sampler2D uNormal;
layout (binding = 3) uniform sampler2D uUv;

layout (binding = 0) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};
layout (binding = 1) uniform CameraUniform
{
  vec3 uCameraPosition;
  vec3 uCameraRotation;
  vec3 uCameraLocalRight;
  vec3 uCameraLocalUp;
  vec3 uCameraLocalFront;
};
layout (binding = 2) uniform PointLightsUniform
{
  LightPoint uPointLights[1024];
};

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

layout (location = 0) out vec4 oColor;

void main()
{
  vec3 fragPosition = texture(uPosition, fragIn.uv).rgb;
  vec3 fragAlbedo = texture(uAlbedo, fragIn.uv).rgb;
  vec3 fragNormal = texture(uNormal, fragIn.uv).rgb;

  vec3 lighting = fragAlbedo * 0.1f;

  vec3 viewDir = normalize(uCameraPosition - fragPosition);

  for (uint i = 0; i < 1024; i++)
  {
    if (uPointLights[i].enabled == 0) continue;

    vec3 lightPosDelta = uPointLights[i].position - fragPosition;
    float lightDist = length(lightPosDelta);

    if (lightDist < uPointLights[i].radius)
    {
      // diffuse
      vec3 lightDir = normalize(lightPosDelta);
      vec3 albedo = max(dot(fragNormal, lightDir), 0.f) * fragAlbedo * uPointLights[i].color.rgb;

      // attenuation
      float attenuation = 1.f / (1.f + uPointLights[i].attenuationLinear * lightDist + uPointLights[i].attenuationQuadratic * lightDist * lightDist);
      
      albedo *= attenuation;

      lighting += albedo;
    }
  }

  oColor = vec4(lighting, 1.f);
}