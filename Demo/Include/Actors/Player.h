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
  Renderer&         mRenderer        { RegistryGetOrCreate<Renderer>("renderer") };

  //FighterLindwurm&  mFighterLindwurm { ACS::CreateChild<FighterLindwurm>(this, 0, "PlayerFighterLindwurm") };
  PlayerController& mPlayerController{ ACS::CreateChild<PlayerController>(this, 1, "PlayerCameraController") };

  Player(Object* pObject)
    : Actor(pObject)
  {
    mTransform.mRotation = { 0.f, 0.f, 45.f };

    mPlayerController.mTransform.mPosition = { 0.f, 0.f, -10.f };
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
};