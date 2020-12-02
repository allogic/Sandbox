#pragma once

#include <Api.h>

#include <Actors/FighterLindwurm.h>
#include <Actors/ScoutWespe.h>

/*
* Patrol actor.
*/

struct Patrol : Actor
{
  u32              mInstanceId;

  FighterLindwurm* mpFighterLindwurm{ ACS::CreateChild<FighterLindwurm>(this, "FighterLindwurm_" + std::to_string(mInstanceId)) };
  ScoutWespe*      mpScoutWespeLB   { ACS::CreateChild<ScoutWespe>(this, "ScoutWespeLB_" + std::to_string(mInstanceId)) };
  ScoutWespe*      mpScoutWespeLT   { ACS::CreateChild<ScoutWespe>(this, "ScoutWespeLT_" + std::to_string(mInstanceId)) };
  ScoutWespe*      mpScoutWespeRB   { ACS::CreateChild<ScoutWespe>(this, "ScoutWespeRB_" + std::to_string(mInstanceId)) };
  ScoutWespe*      mpScoutWespeRT   { ACS::CreateChild<ScoutWespe>(this, "ScoutWespeRT_" + std::to_string(mInstanceId)) };

  Patrol(std::string const& name, Object* pObject, u32 instanceId)
    : mInstanceId{ instanceId }
    , Actor(name, pObject)
  {
    mTransform.mPosition = glm::sphericalRand(1000.f);
    mTransform.mRotation = { glm::linearRand(0.f, 360.f), glm::linearRand(0.f, 360.f), glm::linearRand(0.f, 360.f) };

    mpFighterLindwurm->mTransform.mScale = { 2.f, 2.f, 2.f };

    mpScoutWespeLB->mTransform.mPosition = { -2.f, -2.f, 0.f };
    mpScoutWespeLB->mTransform.mRotation = { 0.f, 0.f, -15.f };
    mpScoutWespeLB->mTransform.mScale = { 2.2f, 2.2f, 2.2f };

    mpScoutWespeLT->mTransform.mPosition = { -2.f, 1.f, 0.f };
    mpScoutWespeLT->mTransform.mRotation = { 0.f, 0.f, 15.f };
    mpScoutWespeLT->mTransform.mScale = { 1.f, 1.f, 1.f };

    mpScoutWespeRB->mTransform.mPosition = { 2.f, -2.f, 0.f };
    mpScoutWespeRB->mTransform.mRotation = { 0.f, 0.f, 15.f };
    mpScoutWespeRB->mTransform.mScale = { 2.2f, 2.2f, 2.2f };

    mpScoutWespeRT->mTransform.mPosition = { 2.f, 1.f, 0.f };
    mpScoutWespeRT->mTransform.mRotation = { 0.f, 0.f, -15.f };
    mpScoutWespeRT->mTransform.mScale = { 1.f, 1.f, 1.f };
  }

  void OnUpdate(r32 time, r32 timeDelta)
  {
    r32v3 delta{ r32v3{ 0.f, 0.f, 0.f } - mTransform.mPosition };
    r32v3 direction{ glm::normalize(delta) };
    r32 distance{ glm::length(delta) };

    if (distance > 100.f)
      mTransform.mPosition += direction * 1000.f * timeDelta;
    else
      mTransform.mPosition += direction * 25.f * timeDelta;

    if (distance < 10.f)
      ACS::DestroyRecursive(this);
  }
};