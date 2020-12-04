#pragma once

#include <Api.h>

#include <AssetDatabase.h>

/*
* ScoutWespe actor.
*/

struct ScoutWespe : Actor
{
  Renderable* mpRenderable{ ACS::Attach<Renderable>(this, ADB::Mesh("Ships\\ScoutWespe.fbx"), ADB::Texture("Ships\\ScoutWespe\\Albedo.png"), ADB::Texture("Ships\\ScoutWespe\\Normal.png"), ADB::Texture("Ships\\ScoutWespe\\Specular.png"), nullptr, nullptr) };

  ScoutWespe(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnGizmo() override
  {
    RendererLineBatchPushMatrix(mRenderer, LocalToWorld());
    RendererLineBatchPushBox(mRenderer, { 0.f, 0.f, 0.f }, { 0.7f, 0.3f, 1.2f }, { 1.f, 1.f, 1.f, 1.f });
    RendererLineBatchPopMatrix(mRenderer);
  }
};