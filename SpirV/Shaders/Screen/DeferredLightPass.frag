#version 460 core

/*
* Constants.
*/

const float PI = 3.14159265359;

/*
* Light layouts.
*/

struct LightPoint
{
  vec3  position;
  float radius;
  vec4  color;
  uint  enabled;
  float attenuationLinear;
  float attenuationQuadratic;
};

/*
* Sampler uniforms.
*/

layout (binding = 0) uniform sampler2D uPosition;
layout (binding = 1) uniform sampler2D uAlbedo;
layout (binding = 2) uniform sampler2D uNormal;
layout (binding = 3) uniform sampler2D uSpecular;
layout (binding = 4) uniform sampler2D uHeight;
layout (binding = 5) uniform sampler2D uMetallic;
layout (binding = 6) uniform sampler2D uRoughness;

/*
* Custom uniforms.
*/

layout (binding = 0) uniform GlobalUniform
{
  float uTime;
  float uTimeDelta;
  vec2  uScreenSize;
};
layout (binding = 1) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};
layout (binding = 2) uniform CameraUniform
{
  vec3 uCameraPosition;
};
layout (binding = 3) uniform PointLightsUniform
{
  LightPoint uPointLights[32];
};

/*
* Vertex input.
*/

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

/*
* Fragment output.
*/

layout (location = 0) out vec4 oColor;

/*
* PBR management.
*/

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness*roughness;
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0f);
  float NdotH2 = NdotH*NdotH;

  float nom   = a2;
  float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
  denom = PI * denom * denom;

  return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = (roughness + 1.0f);
  float k = (r*r) / 8.0f;

  float nom   = NdotV;
  float denom = NdotV * (1.0f - k) + k;

  return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0f);
  float NdotL = max(dot(N, L), 0.0f);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
  return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

/*
* Entry point.
*/

void main()
{
  vec3 fragPosition = texture(uPosition, fragIn.uv).rgb;
  vec3 fragAlbedo = texture(uAlbedo, fragIn.uv).rgb;
  vec3 fragNormal = texture(uNormal, fragIn.uv).rgb;
  float fragSpecular = texture(uSpecular, fragIn.uv).r;
  float fragHeight = texture(uHeight, fragIn.uv).r;
  float fragMetallic = texture(uMetallic, fragIn.uv).r;
  float fragRoughness = texture(uRoughness, fragIn.uv).r;

  vec3 N = normalize(fragNormal);
  vec3 V = normalize(uCameraPosition - fragPosition);

  // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
  // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
  vec3 F0 = vec3(0.04f); 
  F0 = mix(F0, fragAlbedo, fragSpecular);

  // reflectance equation
  vec3 Lo = vec3(0.f);
  for (uint i = 0; i < 32; i++)
  {
    if (uPointLights[i].enabled == 0) continue;

    vec3 L = normalize(uPointLights[i].position - fragPosition);
    vec3 H = normalize(V + L);

    float distance = length(uPointLights[i].position - fragPosition);
    float attenuation = 1.0f / (distance * distance * uPointLights[i].attenuationQuadratic);
    vec3 radiance = uPointLights[i].color.rgb * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, fragRoughness);   
    float G   = GeometrySmith(N, V, L, fragRoughness);      
    vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
    vec3 specular     = nominator / max(denominator, 0.001f); // prevent divide by zero for NdotV=0.0 or NdotL=0.0
    
    // kS is equal to Fresnel
    vec3 kS = F;

    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0f) - kS;

    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0f - fragMetallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0f);

    // add to outgoing radiance Lo
    Lo += (kD * fragAlbedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
  }

  // ambient lighting (note that the next IBL tutorial will replace
  // this ambient lighting with environment lighting).
  vec3 ambient = vec3(0.03f) * fragAlbedo; // * ao;

  vec3 color = ambient + Lo;

  // HDR tonemapping
  color = color / (color + vec3(1.0f));
  // gamma correct
  color = pow(color, vec3(1.0f/2.2f)); 

  oColor = vec4(color, 1.0f);
}