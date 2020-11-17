#version 460 core

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iUv;

layout (location = 0) out VertOut
{
  vec2 uv;
} vertOut;

void main()
{
  vertOut.uv = iUv;

  gl_Position = vec4(iPosition, 1.f);
}