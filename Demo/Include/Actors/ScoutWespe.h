#pragma once

#include <Api.h>

#include <ModelDatabase.h>

/*
* ScoutWespe actor.
*/

struct ScoutWespe : Actor
{
  Renderer&   mRenderer   { RegistryGetOrCreate<Renderer>("renderer") };

  Renderable* mpRenderable{ ACS::Attach<Renderable>(this, &ModelDatabaseGet("ScoutWespe").mMesh, &ModelDatabaseGet("ScoutWespe").mTextureAlbedo, &ModelDatabaseGet("ScoutWespe").mTextureSpecular, nullptr, nullptr, nullptr) };

  ScoutWespe(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnGizmo() override
  {
    r32m4 worldTransform{ glm::identity<r32m4>() };

    if (mpParent)
    {
      worldTransform = TransformTo(mpParent->mTransform.mPosition, mpParent->mTransform.mRotation, mpParent->mTransform.mScale);
    }

    r32m4 localTransform{ TransformTo(mTransform.mPosition, mTransform.mRotation, mTransform.mScale) };

    RendererLineBatchPushMatrix(mRenderer, worldTransform * localTransform);
    RendererLineBatchPushBox(mRenderer, { 0.f, 0.f, 0.f }, { 0.7f, 0.3f, 1.2f }, { 1.f, 1.f, 1.f, 1.f });
    RendererLineBatchPopMatrix(mRenderer);
  }
};