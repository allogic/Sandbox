#version 460 core

struct Transform
{
  float position[3];
  float rotationLocalRight[3];
  float rotationLocalUp[3];
  float rotationLocalFront[3];
};

layout (binding = 0, std430) buffer TransformBuffer
{
  Transform transforms[];
};

layout (binding = 0) uniform ProjectionBlock
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

vec3 ToVec3(in float a[3])
{
  return vec3(a[0], a[1], a[2]);
}
mat4 Rotate3D(vec3 axis, in float angle)
{
  axis = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0f - c;

  return mat4(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.f,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.f,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.f,
    0.f,                                0.f,                                0.f,                                1.f
  );
}

void main()
{
  uint objIndex = gl_InstanceID;

  vertOut.fPosition = lPosition;
  vertOut.fNormal = lNormal;
  vertOut.fUv = lUv;
  vertOut.fColor = lColor;

  vec3 transformPosition = ToVec3(transforms[objIndex].position);

  gl_Position = uProjection * uView * uTransform * vec4(lPosition + transformPosition, 1.f);
}