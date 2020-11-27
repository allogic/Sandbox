#pragma once

#include <Api.h>

#include <ShipDatabase.h>

struct PlayerManager
{
  PlayerManager();
  virtual ~PlayerManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  Ship&                 mShipCruiserBerlin    { ShipDatabaseGet("CruiserBerlin") };
  Ship&                 mShipFighterAtlas     { ShipDatabaseGet("FighterAtlas") };
  Ship&                 mShipFighterLindwurm  { ShipDatabaseGet("FighterLindwurm") };
  Ship&                 mShipScoutAdler       { ShipDatabaseGet("ScoutAdler") };
  Ship&                 mShipScoutWespe       { ShipDatabaseGet("ScoutWespe") };

  Context const&        mContext              { RegistryGetOrCreate<Context>("context") };
  Camera&               mCamera               { RegistryGetOrCreate<Camera>("camera") };
  Renderer&             mRenderer             { RegistryGetOrCreate<Renderer>("renderer") };

  MeshLambert           mMeshRenderTexture    {};

  CameraControllerSpace mCameraControllerSpace{};
  CameraControllerOrbit mCameraControllerOrbit{};
};