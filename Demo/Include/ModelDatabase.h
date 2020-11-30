#pragma once

#include <Api.h>

/*
* Model layouts.
*/

struct Model
{
  MeshLambert    mMesh            {};
  TextureR32RGBA mTextureAlbedo   {};
  TextureR32RGBA mTextureNormal   {};
  TextureR32RGBA mTextureSpecular {};
  TextureR32RGBA mTextureMetallic {};
  TextureR32RGBA mTextureRoughness{};
};

static std::map<std::string, Model> sModelRegistry{};

/*
* Ship management.
*/

static Model& ModelDatabaseGet(std::string const& modelName)
{
  auto const modelIt{ sModelRegistry.find(modelName) };

  if (modelIt == sModelRegistry.end())
  {
    Model model{};

    MeshFrom(model.mMesh, SANDBOX_ROOT_PATH "Models\\Ships\\" + modelName + ".fbx");

    TextureFrom(model.mTextureAlbedo, SANDBOX_ROOT_PATH "Textures\\Ships\\" + modelName + "\\Albedo.png");
    TextureFrom(model.mTextureNormal, SANDBOX_ROOT_PATH "Textures\\Ships\\" + modelName + "\\Normal.png");
    TextureFrom(model.mTextureSpecular, SANDBOX_ROOT_PATH "Textures\\Ships\\" + modelName + "\\Specular.png");

    auto const [insertIt, _]{ sModelRegistry.emplace(modelName, model) };
    return insertIt->second;
  }

  return modelIt->second;
}