#pragma once

#include <Api.h>

/*
* Model layouts.
*/

static std::map<std::filesystem::path, MeshLambert>    sMeshRegistry   {};
static std::map<std::filesystem::path, TextureR32RGBA> sTextureRegistry{};

/*
* Asset database.
*/

namespace ADB
{
  /*
  * Helper utilities.
  */

  static void DebugAssetRegistry()
  {
    ImGui::Begin("ADB");

    if (ImGui::TreeNodeEx("Meshes"))
    {
      for (auto const& [assetPath, mesh] : sMeshRegistry)
      {
        if (ImGui::TreeNodeEx(&mesh, 0, "%s", assetPath.string().c_str()))
        {
          ImGui::TreePop();
        }
      }

      ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Textures"))
    {
      for (auto const& [assetPath, texture] : sTextureRegistry)
      {
        if (ImGui::TreeNodeEx(&texture, 0, "%s", assetPath.string().c_str()))
        {
          ImGui::Image((void*)(std::intptr_t)texture.mTid, { 256, 256 });
          ImGui::TreePop();
        }
      }

      ImGui::TreePop();
    }

    ImGui::End();
  }

  /*
  * Asset management.
  */

  static MeshLambert*    Mesh(std::filesystem::path const& meshPath)
  {
    auto const meshIt{ sMeshRegistry.find(meshPath) };

    if (meshIt == sMeshRegistry.end())
    {
      MeshLambert mesh{};

      MeshFrom(mesh, SANDBOX_ROOT_PATH "Models\\" + meshPath.string());

      auto const [insertIt, _] { sMeshRegistry.emplace(meshPath, mesh) };
      return &insertIt->second;
    }

    return &meshIt->second;
  }
  static TextureR32RGBA* Texture(std::filesystem::path const& texturePath)
  {
    auto const textureIt{ sTextureRegistry.find(texturePath) };

    if (textureIt == sTextureRegistry.end())
    {
      TextureR32RGBA texture{};

      TextureFrom(texture, SANDBOX_ROOT_PATH "Textures\\" + texturePath.string());

      auto const [insertIt, _] { sTextureRegistry.emplace(texturePath, texture) };
      return &insertIt->second;
    }

    return &textureIt->second;
  }
}