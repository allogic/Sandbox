#pragma once

#include <Api.h>

#include <AssetDatabase.h>

/*
* Asteroid field actor.
*/

struct AsteroidField : Actor
{
  u32                     mNumInstances;

  std::vector<Transform>  mTransforms      {};

  BufferLayout<Transform> mBufferTransforms{};

  ShaderCompute           mComputePhysics  {};

  RenderableInstanced*    mpRenderable     { ACS::Attach<RenderableInstanced>(this, &mBufferTransforms, ADB::Mesh("Asteroids\\Asteroid_0.fbx"), ADB::Texture("Ships\\FighterLindwurm\\Albedo.png"), nullptr, nullptr, nullptr, nullptr) };

  AsteroidField(std::string const& name, Object* pObject, u32 numInstances)
    : mNumInstances{ numInstances }
    , Actor(name, pObject)
  {
    BufferLayoutCreate(mBufferTransforms, mNumInstances);

    ShaderLayoutCreate(mComputePhysics, ShaderPaths
    {
      .mCompute{ SANDBOX_ROOT_PATH "SpirV\\Compiled\\Compute\\AsteroidPhysic.comp" },
    });

    InitializeTransforms();

    BufferLayoutBind(mBufferTransforms);
    BufferLayoutDataSet(mBufferTransforms, mNumInstances, mTransforms.data());
    BufferLayoutUnbind();
  }

  void OnUpdateFixed(r32 time, r32 timeDelta) override
  {
    BufferLayoutMap(mBufferTransforms, 0);

    ShaderLayoutBind(mComputePhysics);
    ShaderLayoutCompute(mComputePhysics, mNumInstances / 32, 0, 0);
  }

  void InitializeTransforms()
  {
    mTransforms.resize(mNumInstances);

    for (u32 i{}; i < mNumInstances; i++)
    {
      r32v3 position{ glm::ballRand(10000.f) };
      r32v3 rotation{ glm::linearRand(0.f, 360.f), glm::linearRand(0.f, 360.f), glm::linearRand(0.f, 360.f) };
      r32v3 scale{ glm::linearRand(0.0001f, 0.005f) };

      mTransforms[i] =
      {
        .mPosition{ position },
        .mRotation{ rotation },
        .mScale   { scale },
      };
    }
  }
};