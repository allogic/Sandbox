#pragma once

#include <Api.h>
#include <Object.h>

struct PlayerManager : Object
{
  PlayerManager();
  virtual ~PlayerManager();

  void OnUpdate(r32 timeDelta) override;
  void OnUpdateFixed(r32 timeDelta) override;
  void OnRender(r32 timeDelta) override;
  void OnGizmos(r32 timeDelta) override;

  void CameraControllerUpdateInputSpace(r32 timeDelta);
  void CameraControllerUpdateInputOrbit(r32 timeDelta);

  Camera&                    mCamera                { RegistryGetOrCreate<Camera>("camera") };
  UniformLayout<Projection>& mUniformProjection     { RegistryGetOrCreate<UniformLayout<Projection>>("uniformProjection") };

  CameraControllerSpace      mCameraControllerSpace {};
  CameraControllerOrbit      mCameraControllerOrbit {};
};