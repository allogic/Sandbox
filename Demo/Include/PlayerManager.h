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

  MeshLambert           mMeshCruiser           {};

  TextureR32RGBA        mTextureCruiserAlbedo  {};
  TextureR32RGBA        mTextureCruiserNormal  {};
  TextureR32RGBA        mTextureCruiserSpecular{};

  Context const&        mContext               { RegistryGetOrCreate<Context>("context") };
  Camera&               mCamera                { RegistryGetOrCreate<Camera>("camera") };
  Renderer&             mRenderer              { RegistryGetOrCreate<Renderer>("renderer") };

  CameraControllerSpace mCameraControllerSpace {};
  CameraControllerOrbit mCameraControllerOrbit {};
};