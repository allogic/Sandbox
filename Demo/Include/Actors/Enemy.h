#pragma once

#include <Api.h>

#include <ShipDatabase.h>

/*
* Cruiser berlin actor.
*/

struct Enemy : Actor
{
  Enemy(Object* pObject, std::string const& shipName)
    : mShipName{ shipName }
    , Actor(pObject) {}

  std::string const& mShipName;

  Transform&  mTransform { ACS::Attach<Transform>(this) };
  LightPoint& mLightPoint{ ACS::Attach<LightPoint>(this) };
  Renderable& mRenderable{ ACS::Attach<Renderable>(this, &ShipDatabaseGet(mShipName).mMesh, &ShipDatabaseGet(mShipName).mTextureAlbedo, &ShipDatabaseGet(mShipName).mTextureSpecular, nullptr, nullptr, nullptr) };

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