#pragma once

/*
* Todo:
*  - Create clean shader SSBO/UBO mapping API.
*  - Create SVO/DAG for fast steering lookups.
*  - Continue deferred rendering pipeline.
*/

#include <Api.h>

#include <PlayerManager.h>
#include <ShipManager.h>
#include <BackgroundManager.h>

struct SceneGame : Scene
{
  PlayerManager     mPlayerManager    {};
  ShipManager       mShipManager      { 2048 * 32, 5, 32, 256 };
  BackgroundManager mBackgroundManager{ 2048 * 32 };

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) override;
  void OnGizmos(r32 timeDelta) override;
};