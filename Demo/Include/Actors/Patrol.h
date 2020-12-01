#pragma once

#include <Api.h>

#include <ModelDatabase.h>

#include <Actors/FighterLindwurm.h>
#include <Actors/ScoutWespe.h>

/*
* ScoutAdler actor.
*/

struct Patrol : Actor
{
  u64 const        mInstanceId     {};

  FighterLindwurm& mFighterLindwurm{ ACS::CreateChild<FighterLindwurm>(this, 0, "FighterLindwurm_" + std::to_string(mInstanceId)) };
  ScoutWespe&      mScoutWespeLB   { ACS::CreateChild<ScoutWespe>(this, 1, "ScoutWepeLB_" + std::to_string(mInstanceId)) };
  ScoutWespe&      mScoutWespeLT   { ACS::CreateChild<ScoutWespe>(this, 2, "ScoutWepeLT_" + std::to_string(mInstanceId)) };
  ScoutWespe&      mScoutWespeRB   { ACS::CreateChild<ScoutWespe>(this, 3, "ScoutWepeRB_" + std::to_string(mInstanceId)) };
  ScoutWespe&      mScoutWespeRT   { ACS::CreateChild<ScoutWespe>(this, 4, "ScoutWepeRT_" + std::to_string(mInstanceId)) };

  Patrol(Object* pObject, u64 instanceId)
    : mInstanceId{ instanceId }
    , Actor(pObject)
  {
    mTransform.mRotation = { 0.f, 0.f, 45.f };

    mFighterLindwurm.mTransform.mScale = { 2.f, 2.f, 2.f };

    mScoutWespeLB.mTransform.mPosition = { -2.f, -2.f, 0.f };
    mScoutWespeLB.mTransform.mRotation = { 0.f, 0.f, -15.f };
    mScoutWespeLB.mTransform.mScale = { 2.2f, 2.2f, 2.2f };

    mScoutWespeLT.mTransform.mPosition = { -2.f, 1.f, 0.f };
    mScoutWespeLT.mTransform.mRotation = { 0.f, 0.f, 15.f };
    mScoutWespeLT.mTransform.mScale = { 1.f, 1.f, 1.f };

    mScoutWespeRB.mTransform.mPosition = { 2.f, -2.f, 0.f };
    mScoutWespeRB.mTransform.mRotation = { 0.f, 0.f, 15.f };
    mScoutWespeRB.mTransform.mScale = { 2.2f, 2.2f, 2.2f };

    mScoutWespeRT.mTransform.mPosition = { 2.f, 1.f, 0.f };
    mScoutWespeRT.mTransform.mRotation = { 0.f, 0.f, -15.f };
    mScoutWespeRT.mTransform.mScale = { 1.f, 1.f, 1.f };
  }
};