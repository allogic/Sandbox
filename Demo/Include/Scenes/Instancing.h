#pragma once

#include <Api.h>

struct Transform
{
  r32v3 mPosition {};
  r32v3 mRotation {};
  r32v3 mScale    {};
  r32m4 mTransform{};
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
  
  layout (local_size_x = 16, local_size_y = 1, local_size_y = 1) in;

  struct Transform
  {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat4 transform;
  };
  struct Steering
  {
    vec3 acceleration;
    vec3 steering;
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

  void main()
  {
    uint idx = gl_LocalInvocationIndex.x;

    //bufferSteering.steerings[idx].position += sin(uTimeDelta * 10.f) * 10.f;
  }
  )glsl"
  };
  std::string const mShaderRenderSpaceShipVertex
  {
  R"glsl(
  #version 460 core
  
  struct Transform
  {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat4 transform;
  };
  struct Steering
  {
    vec3 acceleration;
    vec3 steering;
  };

  layout (std430, binding = 0) buffer TransformBuffer
  {
    Transform transforms[];
  };
  layout (std430, binding = 1) buffer SteeringBuffer
  {
    Steering steerings[];
  };

  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec4 lColor;
  layout (location = 3) in mat4 lInstanceId; // refactor me to u32

  uniform mat4 uProjection;
  uniform mat4 uView;
  
  out vec3 fNormal;
  out vec4 fColor;
  
  void main()
  {
    fNormal = lNormal;
    fColor = lColor;

    gl_Position = uProjection * uView * vec4(lPosition + transforms[0].position, 1.f);
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

  constexpr static u32 sNumSpaceShips{ 1000 };

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