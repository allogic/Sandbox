#pragma once

#include <Api.h>

#include <ModelDatabase.h>

/*
* Cruiser berlin actor.
*/

struct Enemy : Actor
{
  std::string const mModelName;

  Transform&        mTransform { ACS::Attach<Transform>(this) };
  LightPoint&       mLightPoint{ ACS::Attach<LightPoint>(this) };
  Renderable&       mRenderable{ ACS::Attach<Renderable>(this, &ModelDatabaseGet(mModelName).mMesh, &ModelDatabaseGet(mModelName).mTextureAlbedo, &ModelDatabaseGet(mModelName).mTextureSpecular, nullptr, nullptr, nullptr) };

  Enemy(Object* pObject, std::string const& modelName)
    : mModelName{ modelName }
    , Actor(pObject)
  {
    mRenderable.mpMeshLambert->mTransform.mScale = { 5.f, 5.f, 5.f };
  }

  void OnUpdate(r32 timeDelta) override
  {

  }
  void OnUpdateFixed(r32 timeDelta) override
  {

  }
  void OnGizmos(r32 timeDelta) override
  {

  }
};