#pragma once

#include <Api.h>

/*
* Player actor.
*/

struct Player : Actor
{
  Context&  mContext { RegistryGetOrCreate<Context>("context") };
  Renderer& mRenderer{ RegistryGetOrCreate<Renderer>("renderer") };

  Player(Object* pObject) : Actor(pObject) {}

  Transform&             mTransform { ACS::Attach<Transform>(this) };
  Camera&                mCamera    { ACS::Attach<Camera>(this) };
  CameraControllerSpace& mController{ ACS::Attach<CameraControllerSpace>(this) };
  //CameraControllerOrbit& mController{ AcsAttach<CameraControllerOrbit>(this) };

  void OnUpdate(r32 timeDelta) override
  {
    CameraControllerUpdateInputSpace(mContext, mCamera, mController, timeDelta);
    //CameraControllerUpdateInputOrbit(mContext, mCamera, mController, timeDelta);
  }
  void OnUpdateFixed(r32 timeDelta) override
  {
    CameraControllerUpdatePhysicsSpace(mTransform, mCamera, mController);
    //CameraControllerUpdatePhysicsOrbit(mTransform, mCamera, mController);
  }
  void OnGizmos(r32 timeDelta) override
  {
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });
  }
};