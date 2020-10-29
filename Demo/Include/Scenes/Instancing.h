#pragma once

#include <Api.h>

struct Transform
{
  r32v3 mPosition{};
  r32v3 mRotationLocalRight{};
  r32v3 mRotationLocalUp{};
  r32v3 mRotationLocalFront{};
};
struct Steering
{
  r32v3 mAcceleration {};
  r32v3 mVelocity     {};
  u32   mWaypointIndex{};
};
struct Waypoint
{
  r32v3 mPosition    {};
  r32v3 mPositionNext{};
};

struct SceneInstancing : Scene
{
  std::string const mShaderComputeSpaceShipUpdate
  {
  R"glsl(
  #version 460 core
  
  layout (local_size_x = 32) in;

  struct Transform
  {
    float position[3];
    float rotationLocalRight[3];
    float rotationLocalUp[3];
    float rotationLocalFront[3];
  };
  struct Steering
  {
    float acceleration[3];
    float velocity[3];
    uint  waypointIndex;
  };
  struct Waypoint
  {
    float position[3];
    float positionNext[3];
  };

  layout (std430, binding = 0) buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) buffer SteeringBuffer
  {
    Steering steerings[];
  };
  layout (std430, binding = 2) buffer WaypointBuffer
  {
    Waypoint waypoints[];
  };

  uniform float uTimeDelta;
  uniform float uAccelerationSpeed;
  uniform float uVelocityDecay;
  uniform uint  uMaxWaypoints;

  mat4 Rotate3D(vec3 axis, float angle)
  {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(
		  oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
      oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
      oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
		  0.0,                                0.0,                                0.0,                                1.0
	  );
  }

  void main()
  {
    uint idx = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    vec3 spaceShipPosition = vec3(transforms[idx].position[0], transforms[idx].position[1], transforms[idx].position[2]);
    uint waypointIndex = steerings[idx].waypointIndex;
    vec3 waypointPosition = vec3(waypoints[waypointIndex].position[0], waypoints[waypointIndex].position[1], waypoints[waypointIndex].position[2]);

    //mat4 roationMatrix = Rotate3D();

    //transforms[idx].rotationLocalRight = rotationMatrix * vec4(transforms[idx].rotationLocalRight, 1.f);
    //transforms[idx].rotationLocalUp = rotationMatrix * vec4(transforms[idx].rotationLocalUp, 1.f);
    //transforms[idx].rotationLocalFront = rotationMatrix * vec4(transforms[idx].rotationLocalFront, 1.f);

    vec3 direction = normalize(vec3(
      waypoints[waypointIndex].position[0] - transforms[idx].position[0],
      waypoints[waypointIndex].position[1] - transforms[idx].position[1],
      waypoints[waypointIndex].position[2] - transforms[idx].position[2]
    ));

    steerings[idx].acceleration[0] += direction.x * uAccelerationSpeed * uTimeDelta;
    steerings[idx].acceleration[1] += direction.y * uAccelerationSpeed * uTimeDelta;
    steerings[idx].acceleration[2] += direction.z * uAccelerationSpeed * uTimeDelta;

    steerings[idx].velocity[0] += steerings[idx].acceleration[0];
    steerings[idx].velocity[1] += steerings[idx].acceleration[1];
    steerings[idx].velocity[2] += steerings[idx].acceleration[2];

    steerings[idx].acceleration[0] = 0.f;
    steerings[idx].acceleration[1] = 0.f;
    steerings[idx].acceleration[2] = 0.f;

    transforms[idx].position[0] += steerings[idx].velocity[0];
    transforms[idx].position[1] += steerings[idx].velocity[1];
    transforms[idx].position[2] += steerings[idx].velocity[2];

    steerings[idx].velocity[0] += -steerings[idx].velocity[0] * uVelocityDecay * uTimeDelta;
    steerings[idx].velocity[1] += -steerings[idx].velocity[1] * uVelocityDecay * uTimeDelta;
    steerings[idx].velocity[2] += -steerings[idx].velocity[2] * uVelocityDecay * uTimeDelta;

    steerings[idx].velocity[0] = clamp(steerings[idx].velocity[0], -1, 1);
    steerings[idx].velocity[1] = clamp(steerings[idx].velocity[1], -1, 1);
    steerings[idx].velocity[2] = clamp(steerings[idx].velocity[2], -1, 1);

    if (length(spaceShipPosition - waypointPosition) < 50.f)
    {
      steerings[idx].waypointIndex += 1;

      if (steerings[idx].waypointIndex >= uMaxWaypoints)
      {
        steerings[idx].waypointIndex = 0;
      }
    }
  }
  )glsl"
  };
  std::string const mShaderRenderSpaceShipVertex
  {
  R"glsl(
  #version 460 core
  
  struct Transform
  {
    float position[3];
    float rotationLocalRight[3];
    float rotationLocalUp[3];
    float rotationLocalFront[3];
  };

  layout (std430, binding = 0) buffer TransformBuffer
  {
    Transform transforms[];
  };

  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec4 lColor;

  uniform mat4 uProjection;
  uniform mat4 uView;
  
  out vec3 fNormal;
  out vec4 fColor;

  void main()
  {
    uint idx = gl_InstanceID;

    fNormal = lNormal;
    fColor = lColor;

    vec3 position = vec3(transforms[idx].position[0], transforms[idx].position[1], transforms[idx].position[2]);

    gl_Position = uProjection * uView * vec4(lPosition + position, 1.f);
  }
  )glsl"
  };
  std::string const mShaderRenderSpaceShipFragment
  {
  R"glsl(
  #version 460 core
  
  in vec3 fNormal;
  in vec4 fColor;
  
  out vec4 color;
  
  void main()
  {
    color = vec4(fNormal, 1.f);
  }
  )glsl"
  };

  u32                     mNumSpaceShips   { 131072 };
  u32                     mNumWaypoints    { 32 };
  CameraControllerOrbit   mCameraController{};
  Model                   mModelSpaceShip  {};
  std::vector<Transform>  mTransforms      {};
  std::vector<Steering>   mSteerings       {};
  std::vector<Waypoint>   mWaypoints       {};
  BufferLayout<Transform> mBufferTransforms{};
  BufferLayout<Steering>  mBufferSteerings {};
  BufferLayout<Waypoint>  mBufferWaypoints {};
  ShaderCompute           mShaderCompute   {};
  ShaderRender            mShaderRender    {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;

  void RandomizeTransforms();
  void RandomizeSteerings();
  void RandomizeWaypoints();
};