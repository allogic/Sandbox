#pragma once

#include <Api.h>

/*
* Ship layouts.
*/

struct Ship
{
  MeshLambert    mMesh            {};
  TextureR32RGBA mTextureAlbedo   {};
  TextureR32RGBA mTextureNormal   {};
  TextureR32RGBA mTextureSpecular {};
  TextureR32RGBA mTextureMetallic {};
  TextureR32RGBA mTextureRoughness{};
};

static std::map<std::string, Ship> sShipRegistry{};

/*
* Ship management.
*/

static Ship& ShipDatabaseGet(std::string const& shipName)
{
  auto const shipIt{ sShipRegistry.find(shipName) };

  if (shipIt == sShipRegistry.end())
  {
    Ship ship{};

    MeshFrom(ship.mMesh, SANDBOX_ROOT_PATH "Models\\Ships\\" + shipName + ".fbx");

    TextureFrom(ship.mTextureAlbedo, SANDBOX_ROOT_PATH "Textures\\Ships\\" + shipName + "\\Albedo.png");
    TextureFrom(ship.mTextureNormal, SANDBOX_ROOT_PATH "Textures\\Ships\\" + shipName + "\\Normal.png");
    TextureFrom(ship.mTextureSpecular, SANDBOX_ROOT_PATH "Textures\\Ships\\" + shipName + "\\Specular.png");

    auto const [insertIt, _]{ sShipRegistry.emplace(shipName, ship) };
    return insertIt->second;
  }

  return shipIt->second;
}