#pragma once

#include <Api.h>

struct SceneModelLoading : Scene
{
  CameraControllerOrbit mCameraController{};
  Model                 mModel           {};
  Shader                mShader          {};

  void OnEnable(r32 aspect) override;
  void OnDisable() override;
  void OnUpdate(r32 deltaTime) override;
  void OnRender() const override;
};