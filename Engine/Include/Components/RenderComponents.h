#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <ADB.h>

#include <Layouts/BufferLayouts.h>

/*
* Render components.
*/

struct Renderable : Component
{
  Renderable(
    Loadable<MeshLambert>*   pMeshLambert,
    Loadable<TextureR32RGB>* pTextureAlbedo,
    Loadable<TextureR32RGB>* pTextureNormal,
    Loadable<TextureR32RGB>* pTextureSpecular,
    Loadable<TextureR32RGB>* pTextureHeight,
    Loadable<TextureR32RGB>* pTextureMetallic,
    Loadable<TextureR32RGB>* pTextureRoughness
  )
    : mpMeshLambert     { pMeshLambert }
    , mpTextureAlbedo   { pTextureAlbedo }
    , mpTextureNormal   { pTextureNormal }
    , mpTextureSpecular { pTextureSpecular }
    , mpTextureHeight   { pTextureHeight }
    , mpTextureMetallic { pTextureMetallic }
    , mpTextureRoughness{ pTextureRoughness } {}

  Loadable<MeshLambert>*   mpMeshLambert;
  Loadable<TextureR32RGB>* mpTextureAlbedo;
  Loadable<TextureR32RGB>* mpTextureNormal;
  Loadable<TextureR32RGB>* mpTextureSpecular;
  Loadable<TextureR32RGB>* mpTextureHeight;
  Loadable<TextureR32RGB>* mpTextureMetallic;
  Loadable<TextureR32RGB>* mpTextureRoughness;
};
struct RenderableInstanced : Component
{
  RenderableInstanced(
    BufferLayout<Transform>*  pBufferTransform,
    Loadable<MeshLambert>*    pMeshLambert,
    Loadable<TextureR32RGBA>* pTextureAlbedo,
    Loadable<TextureR32RGBA>* pTextureNormal,
    Loadable<TextureR32RGBA>* pTextureSpecular,
    Loadable<TextureR32RGBA>* pTextureHeight,
    Loadable<TextureR32RGBA>* pTextureMetallic,
    Loadable<TextureR32RGBA>* pTextureRoughness
  )
    : mpBufferTransform { pBufferTransform }
    , mpMeshLambert     { pMeshLambert }
    , mpTextureAlbedo   { pTextureAlbedo }
    , mpTextureNormal   { pTextureNormal }
    , mpTextureSpecular { pTextureSpecular }
    , mpTextureHeight   { pTextureHeight }
    , mpTextureMetallic { pTextureMetallic }
    , mpTextureRoughness{ pTextureRoughness } {}

  BufferLayout<Transform>*  mpBufferTransform;
  Loadable<MeshLambert>*    mpMeshLambert;
  Loadable<TextureR32RGBA>* mpTextureAlbedo;
  Loadable<TextureR32RGBA>* mpTextureNormal;
  Loadable<TextureR32RGBA>* mpTextureSpecular;
  Loadable<TextureR32RGBA>* mpTextureHeight;
  Loadable<TextureR32RGBA>* mpTextureMetallic;
  Loadable<TextureR32RGBA>* mpTextureRoughness;
};