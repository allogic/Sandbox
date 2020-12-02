#pragma once

#include <Api.h>

#include <Actors/Patrol.h>

/*
* Patrol group actor.
*/

struct PatrolGroup : Actor
{
  u32     mInstanceId;

  Patrol* pPatrolA   { ACS::CreateChild<Patrol>(this, "Patrol_" + std::to_string(mInstanceId + 0), mInstanceId + 0) };
  Patrol* pPatrolB   { ACS::CreateChild<Patrol>(this, "Patrol_" + std::to_string(mInstanceId + 1), mInstanceId + 1) };

  PatrolGroup(std::string const& name, Object* pObject, u32 instanceId)
    : mInstanceId{ instanceId }
    , Actor(name, pObject)
  {
    pPatrolA->mTransform.mPosition = { -20.f, 0.f, 0.f };

    pPatrolB->mTransform.mPosition = { 20.f, 0.f, 0.f };
  }
};