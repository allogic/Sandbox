#pragma once

#include <Api.h>

#include <AssetDatabase.h>

/*
* FighterLindwurm actor.
*/

struct FighterLindwurm : Actor
{
  Renderable* mpRenderable{ ACS::Attach<Renderable>(this, ADB::Mesh("Ships\\FighterLindwurm.fbx"), ADB::Texture("Ships\\FighterLindwurm\\Albedo.png"), ADB::Texture("Ships\\FighterLindwurm\\Normal.png"), ADB::Texture("Ships\\FighterLindwurm\\Specular.png"), nullptr, nullptr) };

  FighterLindwurm(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnGizmo() override
  {
    RendererLineBatchPushMatrix(mRenderer, LocalToWorld());
    RendererLineBatchPushBox(mRenderer, { 0.f, 0.f, 0.f }, { 0.7f, 0.3f, 1.2f }, { 1.f, 1.f, 1.f, 1.f });
    RendererLineBatchPopMatrix(mRenderer);
  }
};