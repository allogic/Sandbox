#pragma once

#include <Api.h>

#include <PlayerManager.h>
#include <ShipManager.h>
#include <BackgroundManager.h>
#include <MapManager.h>

struct SceneGame : Scene
{
  PlayerManager     mPlayerManager    {};
  ShipManager       mShipManager      { 64 * 32, 5, 32, 256 };
  BackgroundManager mBackgroundManager{ 2048 * 32 };
  MapManager        mMapManager       {};

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender() override;
  void OnGizmos(r32 timeDelta) override;
};