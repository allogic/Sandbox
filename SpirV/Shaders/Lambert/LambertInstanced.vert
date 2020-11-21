#version 460 core

struct Transform
{
  float position[3];
  float rotation[3];
  float localRight[3];
  float localUp[3];
  float localFront[3];
};

layout (binding = 0, std430) buffer TransformBuffer
{
  Transform transforms[];
};

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

vec3 ToVec3(in float a[3])
{
  return vec3(a[0], a[1], a[2]);
}
void SetTo(inout float a[3], in vec3 b)
{
  a[0] = b.x;
  a[1] = b.y;
  a[2] = b.z;
}
mat4 Rotate3D(in vec3 axis, in float angle)
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

  // Transform helper references
  vec3 transformPosition = ToVec3(transforms[objIndex].position);
  vec3 transformRotation = ToVec3(transforms[objIndex].rotation);
  vec3 transformLocalRight = ToVec3(transforms[objIndex].localRight);
  vec3 transformLocalUp = ToVec3(transforms[objIndex].localUp);
  vec3 transformLocalFront = ToVec3(transforms[objIndex].localFront);

  // Compute transform matrix
  mat4 tvp = uProjection * uView * uTransform;

  // Compute rotation matrix
  mat4 localRotation = mat4(1.f);
  localRotation = Rotate3D(transformLocalRight, transformRotation.x);
  localRotation = Rotate3D(transformLocalUp, transformRotation.y);
  localRotation = Rotate3D(transformLocalFront, transformRotation.z);

  // Compute vertex rotation in local space
  vec3 vertexPosition = vec4(localRotation * vec4(iPosition, 1.f)).xyz;

  // Forward fragment shader
  vertOut.position = vec4(uTransform * vec4(vertexPosition + transformPosition, 1.f)).xyz;
  vertOut.normal = iNormal;
  vertOut.uv = iUv;
  vertOut.color = iColor;

  // Final position
  gl_Position = tvp * vec4(vertexPosition + transformPosition, 1.f);
}