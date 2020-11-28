#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Render components.
*/

struct Renderable : Component
{
  Renderable(
    MeshLambert*    pMeshLambert,
    TextureR32RGBA* pTextureAlbedo,
    TextureR32RGBA* pTextureNormal,
    TextureR32RGBA* pTextureSpecular,
    TextureR32RGBA* pTextureMetallic,
    TextureR32RGBA* pTextureRoughness
  )
    : mpMeshLambert     { pMeshLambert }
    , mpTextureAlbedo   { pTextureAlbedo }
    , mpTextureNormal   { pTextureNormal }
    , mpTextureSpecular { pTextureSpecular }
    , mpTextureMetallic { pTextureMetallic }
    , mpTextureRoughness{ pTextureRoughness } {}

  MeshLambert*    mpMeshLambert     {};
  TextureR32RGBA* mpTextureAlbedo   {};
  TextureR32RGBA* mpTextureNormal   {};
  TextureR32RGBA* mpTextureSpecular {};
  TextureR32RGBA* mpTextureMetallic {};
  TextureR32RGBA* mpTextureRoughness{};
};