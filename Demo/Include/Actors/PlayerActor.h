#pragma once

#include <Api.h>

#include <Actors/PlayerControllerActor.h>

#include <Actors/Ships/FighterLindwurmActor.h>

/*
* Player actor.
*/

struct Player : Actor
{
  FighterLindwurm*  mpFighterLindwurm { ACS::CreateChild<FighterLindwurm>(this, "PlayerFighterLindwurm") };
  PlayerController* mpPlayerController{ ACS::CreateChild<PlayerController>(this, "PlayerCameraController") };

  Player(std::string const& name, Object* pObject)
    : Actor(name, pObject)
  {
    mpPlayerController->mTransform.mPosition = { 0.f, 1.f, -200.f };
  }

  void OnGizmo() override
  {
    RendererLineBatchPushMatrix(mRenderer, LocalToWorld());
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });
    RendererLineBatchPopMatrix(mRenderer);
  }
};