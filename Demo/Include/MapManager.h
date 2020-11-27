#pragma once

#include <Api.h>

#include <SystemDatabase.h>

struct MapManager
{
  MapManager();
  virtual ~MapManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  System&        mSystemHome   { SystemDatabaseGet("home") };

  Renderer&      mRenderer     { RegistryGetOrCreate<Renderer>("renderer") };

  MeshLambert    mMeshPlanet   {};
  TextureR32RGBA mTexturePlanet{};
};