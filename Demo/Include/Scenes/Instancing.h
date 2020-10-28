#pragma once

#include <Api.h>

struct Steering
{
  r32v3 mPosition {};
  r32v3 mDirection{};
  r32v3 mAccel    {};
};

struct SceneInstancing : Scene
{
  std::string const mShaderComputeSpaceShipSteering
  {
  R"glsl(
  #version 460 core
  
  layout (local_size_x = 16, local_size_y = 1, local_size_y = 1) in;

  struct Steering
  {
    vec3 position;
    vec3 direction;
    vec3 accel;
  };

  layout (std430, binding = 0) buffer BufferSteering
  {
    Steering steerings[];
  } bufferSteering;
  
  uniform float uTimeDelta;

  void main()
  {
    uint idx = gl_LocalInvocationIndex.x;

    bufferSteering.steerings[idx].position += sin(uTimeDelta * 10.f) * 10.f;
  }
  )glsl"
  };
  std::string const mShaderRenderSpaceShipVertex
  {
  R"glsl(
  #version 460 core
  
  struct Steering
  {
    vec3 position;
    vec3 direction;
    vec3 accel;
  };

  layout (std430, binding = 0) buffer BufferSteering
  {
    Steering steerings[];
  } bufferSteering;

  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec4 lColor;
  layout (location = 3) in mat4 lTransform;  

  uniform mat4 uProjection;
  uniform mat4 uView;
  
  out vec3 fPosition;
  out vec3 fNormal;
  out vec4 fColor;
  
  void main()
  {
    fPosition = lPosition;
    fNormal = lNormal;
    fColor = lColor;
  
    gl_Position = uProjection * uView * lTransform * vec4(fPosition, 1.0f);
  }
  )glsl"
  };
  std::string const mShaderRenderSpaceShipFragment
  {
  R"glsl(
  #version 460 core
  
  in vec3 fPosition;
  in vec3 fNormal;
  in vec4 fColor;
  
  out vec4 color;
  
  void main()
  {
    color = vec4(fNormal, 1.0f);
  }
  )glsl"
  };

  constexpr static u32 sNumSpaceShips{ 100000 };

  CameraControllerOrbit  mCameraController{};
  ModelInstanced         mSpaceShip       {};
  std::vector<Steering>  mSteerings       {};
  BufferLayout<Steering> mBufferSteering  {};
  ShaderCompute          mShaderCompute   {};
  ShaderRender           mShaderRender    {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;

  void RandomizeSteerings();
};