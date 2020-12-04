#pragma once

#include <Api.h>

#include <AssetDatabase.h>

/*
* Debug actor.
*/

struct Debug : Actor
{
  Camera* mpCamera{ ACS::Find<Camera>() };

  Debug(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnGizmo() override
  {
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
    RendererLineBatchPushLine(mRenderer, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });

    for (u32 i{}; i < 32; i++)
    {
      r32v3 lightPosition{ mRenderer.mUniformBlockPointLights[i].mPosition };
      r32 lightRadius{ mRenderer.mUniformBlockPointLights[i].mRadius };

      RendererLineBatchPushLine(mRenderer, lightPosition, { lightPosition.x, 0.f, lightPosition.z }, { 1.f, 1.f, 1.f, 1.f });
      RendererLineBatchPushBox(mRenderer, lightPosition, { lightRadius * 2.f, lightRadius * 2.f, lightRadius * 2.f }, { 1.f, 1.f, 1.f, 1.f });
    }

    u32 size{ 32 };
    r32v3 spacing{ 10.f, 10.f, 10.f };

    for (u32 i{}; i <= size; i++)
      for (u32 j{}; j <= size; j++)
      {
        r32v3 half{ size / 2, 0.f, size / 2 };
        r32v3 startP0{ i, 0, 0 };
        r32v3 startP1{ 0, 0, j };
        r32v3 endP0{ i, 0, size };
        r32v3 endP1{ size, 0, j };
        r32v4 color{ 1.f, 1.f, 0.f, 1.f };

        startP0 *= spacing;
        startP1 *= spacing;
        startP0 += -half * spacing;
        startP1 += -half * spacing;

        endP0 *= spacing;
        endP1 *= spacing;
        endP0 += -half * spacing;
        endP1 += -half * spacing;

        RendererLineBatchPushLine(mRenderer, startP0, endP0, color);
        RendererLineBatchPushLine(mRenderer, startP1, endP1, color);
      }
  }
  void OnImGui() override
  {
    ACS::DebugTreeActors();
    ADB::DebugAssetRegistry();
  }
};