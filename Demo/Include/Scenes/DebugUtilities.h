#pragma once

#include <Api.h>

struct SceneDebugUtilities : Scene
{
  void OnEnable(r32 aspect) override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnRender() const override;
};