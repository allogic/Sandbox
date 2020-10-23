#pragma once

#include <Api.h>

struct SceneRayTracing : Scene
{
  CameraControllerOrbit mCameraController{};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 deltaTime) override;
  void OnRender() const override;
};