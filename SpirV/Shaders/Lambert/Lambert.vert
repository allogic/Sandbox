#version 460 core

layout (binding = 2) uniform ProjectionBlock
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec3 lNormal;
layout (location = 2) in vec2 lUv;
layout (location = 3) in vec4 lColor;

layout (location = 0) out VertOut
{
  vec3 fPosition;
  vec3 fNormal;
  vec2 fUv;
  vec4 fColor;
} vertOut;

void main()
{
  vertOut.fPosition = lPosition;
  vertOut.fNormal = lNormal;
  vertOut.fUv = lUv;
  vertOut.fColor = lColor;

  gl_Position = uProjection * uView * uTransform * vec4(lPosition, 1.f);
}