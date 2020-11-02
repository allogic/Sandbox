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
  r32v3 mAcceleration{};
  r32v3 mVelocity    {};
  u32   mPathIndex   {};
  u32   mPathIndexSub{};
};
struct Waypoint
{
  r32v3 mPosition {};
  r32v3 mDirection{};
  r32v3 mRotationLocalFront{};
};

struct SceneGame : Scene
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
    uint  pathIndex;
    uint  pathIndexSub;
  };
  struct Waypoint
  {
    float position[3];
    float direction[3];
    float rotationLocalFront[3];
  };

  layout (std430, binding = 0) volatile restrict buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) volatile restrict buffer SteeringBuffer
  {
    Steering steerings[];
  };
  layout (std430, binding = 2) volatile restrict buffer PathBuffer
  {
    Waypoint paths[];
  };

  uniform float uTimeDelta;
  uniform float uAccelerationSpeed;
  uniform float uVelocityDecay;
  uniform uint  uMaxPaths;

  vec3 ToVec3(in float a[3])
  {
    return vec3(a[0], a[1], a[2]);
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
  vec3 Sub(in float a[3], in float b[3])
  {
    return vec3(
      a[0] - b[0],
      a[1] - b[1],
      a[2] - b[2]
    );
  }
  vec3 Sub(in vec3 a, in float b[3])
  {
    return vec3(
      a.x - b[0],
      a.y - b[1],
      a.z - b[2]
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

    // Compute current path target
    uint pathIndex = steerings[objIndex].pathIndex;
    uint pathIndexSub = steerings[objIndex].pathIndexSub;
    vec3 pathTargetSub = ToVec3(paths[pathIndex * pathIndexSub * 32].position);

    // Add boids cohesion/seperation/alignment behaviour

    // Compute steering direction
    vec3 steeringDirection = Sub(pathTargetSub, transforms[objIndex].position);
    vec3 steeringDirectionNorm = normalize(steeringDirection);

    // Add steering direction to acceleration
    AddTo(steerings[objIndex].acceleration, steeringDirectionNorm * uAccelerationSpeed * uTimeDelta);

    // Add negative steering velocity to itself
    //AddTo(steerings[objIndex].velocity, -steerings[objIndex].velocity * uVelocityDecay * uTimeDelta);

    // Limit velocity
    Clamp(steerings[objIndex].velocity, -1, 1);

    // Loop paths
    if (length(steeringDirection) < 100.f)
      steerings[objIndex].pathIndex = (pathIndex + 1) % uMaxPaths;
  }
  )glsl"
  };
  std::string const mShaderComputeShipPhysicsSource
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
    uint  pathIndex;
    uint  pathIndexSub;
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
  std::string const mShaderComputeShipPathsSource
  {
  R"glsl(
  #version 460 core

  #define PI            3.14159265
  #define NUM_OCTAVES   2

  layout (local_size_x = 32) in;

  struct Waypoint
  {
    float position[3];
    float direction[3];
    float rotationLocalFront[3];
  };

  layout (std430, binding = 2) volatile restrict buffer PathBuffer
  {
    Waypoint paths[];
  };

  uniform float uWindowSizeX;
  uniform vec3  uOffsetRandom;

  // Helper functions
  vec3 ToVec3(in float a[3])
  {
    return vec3(a[0], a[1], a[2]);
  }
  void AddTo(inout float a[3], in vec3 b)
  {
    a[0] += b.x;
    a[1] += b.y;
    a[2] += b.z;
  }
  void SetTo(inout float a[3], in vec3 b)
  {
    a[0] = b.x;
    a[1] = b.y;
    a[2] = b.z;
  }

  // Generic noise
  float Hash(in float n)
  {
    return fract(sin(n) * 1e4);
  }
  float Hash(in vec2 p)
  {
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
  }
  float Noise(in float x)
  {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(Hash(i), Hash(i + 1.0), u);
  }
  float Noise(in vec2 x)
  {
    vec2 i = floor(x);
    vec2 f = fract(x);

    // Four corners in 2D of a tile
    float a = Hash(i);
    float b = Hash(i + vec2(1.0, 0.0));
    float c = Hash(i + vec2(0.0, 1.0));
    float d = Hash(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
  }
  float Noise(in vec3 x)
  {
  	const vec3 step = vec3(110, 241, 171) + uOffsetRandom;
  
  	vec3 i = floor(x);
  	vec3 f = fract(x);
   
  	// For performance, compute the base input to a 1D hash from the integer part of the argument and the 
  	// incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);
  
  	vec3 u = f * f * (3.0 - 2.0 * f);
  	return mix(mix(mix( Hash(n + dot(step, vec3(0, 0, 0))), Hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( Hash(n + dot(step, vec3(0, 1, 0))), Hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( Hash(n + dot(step, vec3(0, 0, 1))), Hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( Hash(n + dot(step, vec3(0, 1, 1))), Hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
  }

  // Fractional brownian
  float Fbm(in float x)
  {
    float v = 0.0;
    float a = 0.5;
    float shift = float(100);
    for (int i = 0; i < NUM_OCTAVES; ++i)
    {
      v += a * Noise(x);
      x = x * 2.0 + shift;
      a *= 0.5;
    }
    return v;
  }
  float Fbm(in vec2 x)
  {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < NUM_OCTAVES; ++i)
    {
      v += a * Noise(x);
      x = rot * x * 2.0 + shift;
      a *= 0.5;
    }
    return v;
  }
  float Fbm(in vec3 x)
  {
    float v = 0.0;
    float a = 0.5;
    vec3 shift = vec3(100);
    for (int i = 0; i < NUM_OCTAVES; ++i)
    {
      v += a * Noise(x);
      x = x * 2.0 + shift;
      a *= 0.5;
    }
    return v;
  }

  void main()
  {
    if (gl_WorkGroupID.x % gl_WorkGroupSize.x == 0 && gl_LocalInvocationID.x == 0)
      return;

    uint pathIndex = gl_GlobalInvocationID.x;

    vec3 positionPrev = ToVec3(paths[pathIndex - 1].position);
    vec3 directionPrev = ToVec3(paths[pathIndex - 1].direction);

    // Compute current sub path position
    vec3 position = positionPrev + directionPrev;
    SetTo(paths[pathIndex].position, position);

    // Compute current direction from gradient
    float angle = Fbm(position) * 10.f;
    mat3 rot = mat3(
       cos(angle), 0, sin(angle),
                0, 1,          0,
      -sin(angle), 0, cos(angle)
    );
    vec3 direction = rot * vec3(directionPrev.x, 0, directionPrev.z);
    vec3 directionNorm = normalize(direction);
    //vec3 directionRand = vec3(sin(angle), 0.f, cos(angle));

    paths[pathIndex].direction[0] = directionNorm.x + 1.f;
    paths[pathIndex].direction[1] = directionNorm.y + 1.f;
    paths[pathIndex].direction[2] = directionNorm.z + 1.f;
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

  uniform mat4 uProjection;
  uniform mat4 uView;

  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec2 lUv;
  layout (location = 3) in vec4 lColor;
  
  out vec3 fNormal;
  out vec2 fUv;
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
    fUv = lUv;
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
  in vec2 fUv;
  in vec4 fColor;
  
  out vec4 color;
  
  void main()
  {
    // Compute fragment color
    color = vec4(fNormal, 1.f);
  }
  )glsl"
  };
  std::string const mShaderRenderSkyVertexSource
  {
  R"glsl(
  #version 460 core

  uniform mat4 uProjection;
  uniform mat4 uView;
  uniform mat4 uTransform;  

  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec2 lUv;
  layout (location = 3) in vec4 lColor;
  
  out vec3 fPosition;
  out vec3 fNormal;
  out vec2 fUv;
  out vec4 fColor;

  void main()
  {
    // Forwarding variables
    fPosition = lPosition;
    fNormal = lNormal;
    fUv = lUv;
    fColor = lColor;

    // Compute screen position
    gl_Position = uProjection * uView * uTransform * vec4(lPosition, 1.f);
  }
  )glsl"
  };
  std::string const mShaderRenderSkyFragmentSource
  {
  R"glsl(
  #version 460 core
  
  uniform sampler2D uDiffuse;

  in vec3 fPosition;
  in vec3 fNormal;
  in vec2 fUv;
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
  u32                     mNumPaths                  { 32 };
  u32                     mNumPathSub                { 1024 };

  CameraControllerOrbit   mCameraController          {};

  ModelLambert            mModelShip                 {};
  ModelLambert            mModelSky                  {};

  TextureU8RGB            mTextureSky                {};

  std::vector<Transform>  mTransforms                {};
  std::vector<Steering>   mSteerings                 {};
  std::vector<Waypoint>   mPaths                     {};

  BufferLayout<Transform> mBufferTransforms          {};
  BufferLayout<Steering>  mBufferSteerings           {};
  BufferLayout<Waypoint>  mBufferPaths               {};

  ShaderCompute           mShaderComputeShipSteerings{};
  ShaderCompute           mShaderComputeShipPhysics  {};
  ShaderCompute           mShaderComputeShipPaths    {};

  ShaderRender            mShaderRenderShips         {};
  ShaderRender            mShaderRenderSky           {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;

  void InitializeTransforms();
  void InitializeSteerings();
  void InitializePaths();
};