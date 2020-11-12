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

struct SceneGame : Scene
{
  SceneGame(u32 width, u32 height) : Scene(width, height) {}

  PlayerManager mPlayerManager{};
  ShipManager   mShipManager  { 2048 * 32, 5, 32, 256 };

  void OnEnable() override;
  void OnDisable() override;
  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) override;
  void OnGizmos(r32 timeDelta) override;

  RenderMaterialLambert mMaterialCruiserBerlin{};
  ModelLambert          mModelCruiserBerlin   {};
};