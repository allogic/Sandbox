#pragma once

#include <Api.h>

struct SceneModelLoading : Scene
{
  CameraControllerSpace mCameraController{};
  Model                 mModel           {};
  Shader                mShader          {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 deltaTime) override;
  void OnRender() const override;
};