#pragma once

#include <Api.h>

struct Velocity
{
  r32v3 mPosition {};
  r32v3 mDirection{};
  r32v3 mAccel    {};
};

struct SceneInstancing : Scene
{
  std::string const mShaderComputeVelocity
  {
  R"glsl(
  #version 460 core
  
  layout (local_size_x = 16, local_size_y = 16) in;
  
  layout (std430, binding = 0) buffer anotherLayoutName
  {
    vec3 position;
    vec3 direction;
    vec3 accel;
  };
  
  void main()
  {
    ivec2 gid = ivec2(gl_GlobalInvocationID.xy);
    ivec2 lid = ivec2(gl_LocalInvocationID.xy);
  }
  )glsl"
  };
  std::string const mShaderRenderVertexLambert
  {
  R"glsl(
  #version 460 core
  
  layout (location = 0) in vec3 lPosition;
  layout (location = 1) in vec3 lNormal;
  layout (location = 2) in vec4 lColor;
  
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
  
    gl_Position = uProjection * uView * vec4(lPosition, 1.0f);
  }
  )glsl"
  };
  std::string const mShaderRenderFragmentLambert
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

  constexpr static u32 sVelocitySize{ 2048 };

  CameraControllerSpace  mCameraController         {};
  Model                  mModel                    {};
  Velocity               mVelocities[sVelocitySize]{};
  BufferLayout<Velocity> mBufferVelocity           {};
  ShaderCompute          mShaderCompute            {};
  ShaderRender           mShaderRender             {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender() const override;
  void OnGizmos(r32 timeDelta) override;

  void RandomizeVelocities();
};