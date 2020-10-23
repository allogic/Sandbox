#pragma once

#include <Api.h>

struct SceneDebugUtilities : Scene
{
  CameraControllerSpace mCameraController{};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnRender() const override;
};