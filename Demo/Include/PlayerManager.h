#pragma once

#include <Api.h>

struct PlayerManager
{
  PlayerManager();
  virtual ~PlayerManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  void CameraControllerUpdateInputSpace(r32 timeDelta);
  void CameraControllerUpdateInputOrbit(r32 timeDelta);

  ModelLambert          mModelCruiser         {};
  TextureR32RGBA        mTextureCruiser       {};

  Camera&               mCamera               { RegistryGetOrCreate<Camera>("camera") };
  Renderer&             mRenderer             { RegistryGetOrCreate<Renderer>("renderer") };

  CameraControllerSpace mCameraControllerSpace{};
  CameraControllerOrbit mCameraControllerOrbit{};
};