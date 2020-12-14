#pragma once

#include <Core.h>
#include <Types.h>
#include <ADB.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/TextureLayouts.h>

/*
* Gui interface for streaming assets.
*/

namespace ADB
{
  static void DebugAssets()
  {
    ImGui::Begin("Assets");

    if (ImGui::TreeNodeEx("Meshes"))
    {
      for (auto const& [assetPath, loadable] : sAsyncMeshDb<MeshLambert>)
      {
        if (ImGui::TreeNodeEx(&loadable.mInstance, 0, "%s", assetPath.c_str()))
        {
          ImGui::Text("Loaded %d", loadable.mStatus == eAsyncStateReady);
          ImGui::TreePop();
        }
      }
    
      ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Textures"))
    {
      for (auto const& [assetPath, loadable] : sAsyncTextureDb<TextureR32RGB>)
      {
        if (ImGui::TreeNodeEx(&loadable.mInstance, 0, "%s", assetPath.c_str()))
        {
          ImGui::Text("Loaded %d", loadable.mStatus == eAsyncStateReady);
          ImGui::Text("Texture ID %d", loadable.mInstance.mTid);
          ImGui::Image((void*)(std::intptr_t)loadable.mInstance.mTid, { 256, 256 });
          ImGui::TreePop();
        }
      }
    
      ImGui::TreePop();
    }

    ImGui::End();
  }
}