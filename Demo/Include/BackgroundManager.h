#pragma once

#include <Api.h>

struct BackgroundManager
{
  BackgroundManager(u32 numStars);
  virtual ~BackgroundManager();

  void Update(r32 timeDelta);
  void UpdatePhysics(r32 timeDelta);
  void Render();
  void Debug();

  void InitializeStarTransforms();

  u32                           mNumStars                {};

  MeshLambert                   mMeshStar                {};

  TextureR32RGBA                mTextureStar             {};

  std::vector<BufferTransform>  mStarTransforms          {};

  BufferLayout<BufferTransform> mBufferStarTransform     {};

  ComputeMaterialDefault        mMaterialComputeStarField{ mNumStars / 32 };

  Renderer&                     mRenderer                { RegistryGetOrCreate<Renderer>("renderer") };
};