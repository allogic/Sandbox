#pragma once

#include <Api.h>

#include <ModelDatabase.h>

#include <Actors/FighterLindwurm.h>
#include <Actors/PlayerController.h>

/*
* Player actor.
*/

struct Player : Actor
{
  Renderer&         mRenderer         { RegistryGetOrCreate<Renderer>("renderer") };

  FighterLindwurm*  mpFighterLindwurm { ACS::CreateChild<FighterLindwurm>(this, "PlayerFighterLindwurm") };
  PlayerController* mpPlayerController{ ACS::CreateChild<PlayerController>(this, "PlayerCameraController") };

  Player(std::string const& name, Object* pObject)
    : Actor(name, pObject)
  {
    mTransform.mRotation = { 0.f, 0.f, 45.f };

    mpPlayerController->mTransform.mPosition = { 0.f, 1.f, -200.f };
  }

  void OnGizmo() override
  {
    r32m4 localTransform{ TransformTo(mTransform.mPosition, mTransform.mRotation, mTransform.mScale) };

    RendererLineBatchPushMatrix(mRenderer, localTransform);
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });
    RendererLineBatchPopMatrix(mRenderer);
  }
  void OnImGui() override
  {
    ACS::DebugTreeActors();
  }
};