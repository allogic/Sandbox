#version 460 core

layout (binding = 0) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iUv;
layout (location = 3) in vec4 iColor;

layout (location = 0) out VertOut
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertOut;

void main()
{
  mat4 tp = uProjection * uTransform;
  mat4 tvp = uProjection * uView * uTransform;

  vertOut.position = vec4(tp * vec4(iPosition, 1.f)).xyz;
  vertOut.normal = iNormal;
  vertOut.uv = iUv;
  vertOut.color = iColor;

  gl_Position = tvp * vec4(iPosition, 1.f);
}