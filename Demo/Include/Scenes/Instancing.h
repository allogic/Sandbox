#pragma once

#include <Api.h>

struct Transform
{
  r32v3 mPosition{};
};
struct Steering
{
  r32v3 mAcceleration{};
  r32v3 mVelocity    {};
};

struct SceneInstancing : Scene
{
  std::string const mShaderComputeSpaceShipUpdate
  {
  R"glsl(
  #version 460 core
  
  layout (local_size_x = 16) in;

  struct Transform
  {
    float position[3];
  };
  struct Steering
  {
    float acceleration[3];
    float velocity[3];
  };

  layout (std430, binding = 0) buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) buffer SteeringBuffer
  {
    Steering steerings[];
  };

  uniform float uTimeDelta;
  uniform float uVelocityDecay;

  void main()
  {
    uint idx = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;

    vec3 acceleration = vec3(steerings[idx].acceleration[0], steerings[idx].acceleration[1], steerings[idx].acceleration[2]);
    vec3 velocity = vec3(steerings[idx].velocity[0], steerings[idx].velocity[1], steerings[idx].velocity[2]);

    steerings[idx].velocity[0] += -velocity.x * uVelocityDecay * uTimeDelta;
    steerings[idx].velocity[1] += -velocity.y * uVelocityDecay * uTimeDelta;
    steerings[idx].velocity[2] += -velocity.z * uVelocityDecay * uTimeDelta;

    steerings[idx].velocity[0] += acceleration.x;
    steerings[idx].velocity[1] += acceleration.y;
    steerings[idx].velocity[2] += acceleration.z;

    //steerings[idx].acceleration[0] = 0.f;
    //steerings[idx].acceleration[1] = 0.f;
    //steerings[idx].acceleration[2] = 0.f;

    transforms[idx].position[0] += velocity.x;
    transforms[idx].position[1] += velocity.y;
    transforms[idx].position[2] += velocity.z;
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

  constexpr static u32 sNumSpaceShips{ 131072 };

  CameraControllerOrbit   mCameraController{};
  Model                   mModelSpaceShip  {};
  std::vector<Transform>  mTransforms      {};
  std::vector<Steering>   mSteerings       {};
  BufferLayout<Transform> mBufferTransforms{};
  BufferLayout<Steering>  mBufferSteerings {};
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
};