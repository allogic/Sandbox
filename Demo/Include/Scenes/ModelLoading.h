#pragma once

#include <Api.h>

struct SceneModelLoading : Scene
{
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

  CameraControllerOrbit mCameraController{};
  Model                 mModel           {};
  ShaderRender          mShaderRender    {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 deltaTime) override;
  void OnUpdateFixed(r32 deltaTime) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;
};