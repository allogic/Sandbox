#version 460 core

layout (binding = 2) uniform ProjectionBlock
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec4 lColor;

layout (location = 0) out VertOut
{
  vec3 fPosition;
  vec4 fColor;
} vertOut;

void main()
{
  vertOut.fPosition = lPosition;
  vertOut.fColor = lColor;

  gl_Position = uProjection * uView * vec4(lPosition, 1.f);
}