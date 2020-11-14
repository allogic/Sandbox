#pragma once

#include <Api.h>

struct PlayerManager
{
  PlayerManager();
  virtual ~PlayerManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);

  void CameraControllerUpdateInputSpace(r32 timeDelta);
  void CameraControllerUpdateInputOrbit(r32 timeDelta);

  Camera&                    mCamera                { RegistryGetOrCreate<Camera>("camera") };
  UniformLayout<Projection>& mUniformProjection     { RegistryGetOrCreate<UniformLayout<Projection>>("uniformProjection") };

  CameraControllerSpace      mCameraControllerSpace {};
  CameraControllerOrbit      mCameraControllerOrbit {};
};