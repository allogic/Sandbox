#pragma once

#include <Api.h>

#include <Actors/Patrol.h>

/*
* Entity spawner actor.
*/

struct EntitySpawner : Actor
{
  r32 mSpawnRate;

  r32 mTimeSpawnPrev{};
  u32 mInstanceId   {};

  EntitySpawner(std::string const& name, Object* pObject, r32 spawnRate)
    : mSpawnRate{ spawnRate }
    , Actor(name, pObject) {}

  void OnUpdate(r32 time, r32 timeDelta) override
  {
    if ((time - mTimeSpawnPrev) >= mSpawnRate)
    {
      ACS::Create<Patrol>(std::to_string(mInstanceId), mInstanceId);

      mInstanceId++;

      mTimeSpawnPrev = time;
    }
  }
};