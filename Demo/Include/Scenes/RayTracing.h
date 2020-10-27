#pragma once

#include <Api.h>

struct SceneRayTracing : Scene
{
  CameraControllerOrbit mCameraController{};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 deltaTime) override;
  void OnUpdateFixed(r32 deltaTime) override;
  void OnRender(r32 timeDelta) const override;
  void OnGizmos(r32 timeDelta) override;
};