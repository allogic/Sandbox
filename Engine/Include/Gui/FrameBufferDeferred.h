#pragma once

#include <Core.h>
#include <Types.h>
#include <FrameBufferDeferred.h>

#include <Layouts/TextureLayouts.h>

/*
* Gui interface for deferred render textures.
*/

namespace FBD
{
  static void DebugDeferredTextures(FrameBufferDeferred const& frameBuffer)
  {
    ImGui::Begin("FBD");

    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTexturePosition.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureAlbedo.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureNormal.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureSpecular.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureHeight.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureMetallic.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureRoughness.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });
    ImGui::Image((void*)(std::intptr_t)frameBuffer.mTextureDepth.mTid, { 352, 240 }, { 0, 1 }, { 1, 0 });

    ImGui::End();
  }
}