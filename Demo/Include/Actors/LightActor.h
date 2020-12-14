#pragma once

#include <Api.h>

/*
* Random light actor.
*/

struct Light : Actor
{
  LightPoint* mpLightPoint{ ACS::Attach<LightPoint>(this) };

  Light(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}

  void OnGizmo() override
  {
    RendererLineBatchPushMatrix(mRenderer, mpTransform->LocalToWorld());
    RendererLineBatchPushBox(mRenderer, mpTransform->mPosition, r32v3{ mpLightPoint->mRadius }, mpLightPoint->mColor);
    RendererLineBatchPopMatrix(mRenderer);
  }
};