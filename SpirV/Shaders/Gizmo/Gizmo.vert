#version 460 core

layout (binding = 0) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec4 iColor;

layout (location = 0) out VertOut
{
  vec4 color;
} vertOut;

void main()
{
  vertOut.color = iColor;

  gl_Position = uProjection * uView * uTransform * vec4(iPosition, 1.f);
}