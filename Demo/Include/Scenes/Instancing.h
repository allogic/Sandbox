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
  std::string const mShaderComputeShipSteeringsSource
  {
  R"glsl(
  #version 460 core

  layout (local_size_x = 64) in;

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

  layout (std430, binding = 0) buffer TransformBuffer{ Transform transforms[]; };
  layout (std430, binding = 1) buffer SteeringBuffer { Steering steerings[]; };
  layout (std430, binding = 2) buffer WaypointBuffer { Waypoint waypoints[]; };

  uniform float uTimeDelta;
  uniform float uSpeed;
  uniform float uDecay;
  uniform uint  uMaxWaypoints;

  vec3 ToVec3(in float arr[3])
  {
    return vec3(arr[0], arr[1], arr[2]);
  }
  void AddTo(inout float arr[3], in vec3 value)
  {
    arr[0] += value.x;
    arr[1] += value.y;
    arr[2] += value.z;
  }
  void AddTo(inout float arr[3], in float value[3])
  {
    arr[0] += value[0];
    arr[1] += value[1];
    arr[2] += value[2];
  }

  void main()
  {
    // Compute linear object index
    uint objIndex = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    // Compute current waypoint position
    uint waypointIndex = steerings[objIndex].waypointIndex;
    vec3 waypointPosition = ToVec3(waypoints[waypointIndex].position);
    
    // Add boids coheasion/seperation/alignment behaviour

    // Compute steering direction
    vec3 transformPosition = ToVec3(transforms[objIndex].position);
    vec3 steeringDirection = waypointPosition - transformPosition;
    vec3 steeringDirectionNorm = normalize(steeringDirection);

    // Add steering direction to acceleration
    AddTo(steerings[objIndex].acceleration, steeringDirectionNorm * uSpeed * uTimeDelta);

    // Add negative steering velocity to itself
    AddTo(steerings[objIndex].velocity, -ToVec3(steerings[objIndex].velocity) * uDecay * uTimeDelta);

    // Loop waypoints steering
    if (length(transformPosition - waypointPosition) < 500.f)
      steerings[objIndex].waypointIndex = waypointIndex + 1 % uMaxWaypoints;
  }
  )glsl"
  };
  std::string const mShaderComputeShipPhysicsSource
  {
  R"glsl(
  #version 460 core
  
  layout (local_size_x = 64) in;

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

  layout (std430, binding = 0) buffer TransformBuffer{ Transform transforms[]; };
  layout (std430, binding = 1) buffer SteeringBuffer { Steering steerings[]; };

  mat4 Rotate3D(vec3 axis, in float angle)
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
  vec3 ToVec3(in float arr[3])
  {
    return vec3(arr[0], arr[1], arr[2]);
  }
  void AddTo(inout float arr[3], in vec3 value)
  {
    arr[0] += value.x;
    arr[1] += value.y;
    arr[2] += value.z;
  }
  void AddTo(inout float arr[3], in float value[3])
  {
    arr[0] += value[0];
    arr[1] += value[1];
    arr[2] += value[2];
  }
  void SetTo(inout float arr[3], in float value)
  {
    arr[0] = value;
    arr[1] = value;
    arr[2] = value;
  }

  void main()
  {
    // Compute linear object index
    uint objIndex = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    // Transform rotation
    //transforms[idx].rotationLocalRight = rotationMatrix * vec4(transforms[idx].rotationLocalRight, 1.f);
    //transforms[idx].rotationLocalUp = rotationMatrix * vec4(transforms[idx].rotationLocalUp, 1.f);
    //transforms[idx].rotationLocalFront = rotationMatrix * vec4(transforms[idx].rotationLocalFront, 1.f);

    // Add velocity to acceleration
    AddTo(steerings[objIndex].velocity, steerings[objIndex].acceleration);

    // Clear velocity
    SetTo(steerings[objIndex].acceleration, 0.f);

    // Add velocity to position
    AddTo(transforms[objIndex].position, steerings[objIndex].velocity);
  }
  )glsl"
  };
  std::string const mShaderRenderShipVertexSource
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

  vec3 ToVec3(in float arr[3])
  {
    return vec3(arr[0], arr[1], arr[2]);
  }

  void main()
  {
    // Object index
    uint objIndex = gl_InstanceID;

    // Forwarding variables
    fNormal = lNormal;
    fColor = lColor;

    vec3 transformPosition = ToVec3(transforms[objIndex].position);

    // Compute screen position
    gl_Position = uProjection * uView * vec4(lPosition + transformPosition, 1.f);
  }
  )glsl"
  };
  std::string const mShaderRenderShipFragmentSource
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

  u32                     mNumShips                  { 4096 * 32 };
  u32                     mNumWaypoints              { 32 };
                                                     
  CameraControllerOrbit   mCameraController          {};
  Model                   mModelShip                 {};
                                                     
  std::vector<Transform>  mTransforms                {};
  std::vector<Steering>   mSteerings                 {};
  std::vector<Waypoint>   mWaypoints                 {};
                                                     
  BufferLayout<Transform> mBufferTransforms          {};
  BufferLayout<Steering>  mBufferSteerings           {};
  BufferLayout<Waypoint>  mBufferWaypoints           {};

  ShaderCompute           mShaderComputeShipSteerings{};
  ShaderCompute           mShaderComputeShipPhysics  {};

  ShaderRender            mShaderRenderShips         {};

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