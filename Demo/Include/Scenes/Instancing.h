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
  r32v3 mPosition{};
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
  };

  layout (std430, binding = 0) volatile restrict buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) volatile restrict buffer SteeringBuffer
  {
    Steering steerings[];
  };
  layout (std430, binding = 2) volatile restrict buffer WaypointBuffer
  {
    Waypoint waypoints[];
  };

  uniform float uTimeDelta;
  uniform float uAccelerationSpeed;
  uniform float uVelocityDecay;
  uniform uint  uMaxWaypoints;

  void AddTo(inout float a[3], in vec3 b)
  {
    a[0] += b.x;
    a[1] += b.y;
    a[2] += b.z;
  }
  void AddTo(inout float a[3], in float b[3])
  {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
  }
  vec3 Sub(in float a[3], in float b[3])
  {
    return vec3(
      a[0] - b[0],
      a[1] - b[1],
      a[2] - b[2]
    );
  }
  void Clamp(inout float a[3], in float min, in float max)
  {
    a[0] = clamp(a[0], min, max);
    a[1] = clamp(a[1], min, max);
    a[2] = clamp(a[2], min, max);
  }

  void main()
  {
    // Compute linear object index
    uint objIndex = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    // Current waypoint index
    uint waypointIndex = steerings[objIndex].waypointIndex;
    
    // Add boids cohesion/seperation/alignment behaviour

    // Compute steering direction
    vec3 steeringDirection = Sub(waypoints[waypointIndex].position, transforms[objIndex].position);
    vec3 steeringDirectionNorm = normalize(steeringDirection);

    // Add steering direction to acceleration
    AddTo(steerings[objIndex].acceleration, steeringDirectionNorm * uAccelerationSpeed * uTimeDelta);

    // Add negative steering velocity to itself
    //AddTo(steerings[objIndex].velocity, -steerings[objIndex].velocity * uVelocityDecay * uTimeDelta);

    // Limit velocity
    Clamp(steerings[objIndex].velocity, -1, 1);

    // Loop waypoints
    if (length(steeringDirection) < 100.f)
      steerings[objIndex].waypointIndex = (waypointIndex + 1) % uMaxWaypoints;
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

  layout (std430, binding = 0) volatile restrict buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) volatile restrict buffer SteeringBuffer
  {
    Steering steerings[];
  };

  mat4 Rotate3D(vec3 axis, in float angle)
  {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0f - c;

    return mat4(
		  oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
      oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
      oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
		  0.0,                                0.0,                                0.0,                                1.0
	  );
  }
  void AddTo(inout float a[3], in vec3 b)
  {
    a[0] += b.x;
    a[1] += b.y;
    a[2] += b.z;
  }
  void AddTo(inout float a[3], in float b[3])
  {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
  }
  void SetTo(inout float a[3], in float b)
  {
    a[0] = b;
    a[1] = b;
    a[2] = b;
  }

  void main()
  {
    // Compute linear object index
    uint objIndex = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    // Transform rotation
    //transforms[idx].rotationLocalRight = rotationMatrix * vec4(transforms[idx].rotationLocalRight, 1.f);
    //transforms[idx].rotationLocalUp = rotationMatrix * vec4(transforms[idx].rotationLocalUp, 1.f);
    //transforms[idx].rotationLocalFront = rotationMatrix * vec4(transforms[idx].rotationLocalFront, 1.f);

    // Add acceleration to velocity
    AddTo(steerings[objIndex].velocity, steerings[objIndex].acceleration);

    // Clear acceleration
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

  layout (std430, binding = 0) volatile restrict buffer TransformBuffer
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

  vec3 ToVec3(in float a[3])
  {
    return vec3(a[0], a[1], a[2]);
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
    // Compute fragment color
    color = vec4(fNormal, 1.f);
  }
  )glsl"
  };

  u32                     mNumShips                  { 2048 * 32 };
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