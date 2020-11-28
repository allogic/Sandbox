#pragma once

#include <Api.h>

#include <ShipDatabase.h>

#include <Actors/Player.h>
#include <Actors/CruiserBerlin.h>

struct PlayerManager
{
  PlayerManager();
  virtual ~PlayerManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  
};