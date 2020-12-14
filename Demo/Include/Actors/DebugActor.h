#pragma once

#include <Api.h>

#include <Actors/LightActor.h>

struct SphereRock : Actor
{
  Renderable* mpRenderable
  {
    ACS::Attach<Renderable>(
      this,
      ADB::Mesh<MeshLambert>("Primitives\\Sphere.obj"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Albedo.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Normal.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Specular.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Bump.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Gloss.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Rock\\Roughness.jpg")
    )
  };

  SphereRock(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}
};
struct SphereMetal : Actor
{
  Renderable* mpRenderable
  {
    ACS::Attach<Renderable>(
      this,
      ADB::Mesh<MeshLambert>("Primitives\\Sphere.obj"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Albedo.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Normal.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Specular.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Bump.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Gloss.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Metal\\Roughness.jpg")
    )
  };

  SphereMetal(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}
};
struct Terrain : Actor
{
  Renderable* mpRenderable
  {
    ACS::Attach<Renderable>(
      this,
      ADB::Mesh<MeshLambert>("Primitives\\Quad.obj"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Albedo.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Normal.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Specular.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Bump.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Gloss.jpg"),
      ADB::Texture<TextureR32RGB>("Quixel\\2048\\Mud\\Roughness.jpg")
    )
  };

  Terrain(std::string const& name, Object* pObject)
    : Actor(name, pObject) {}
};

/*
* Debug actor.
*/

struct Debug : Actor
{
  Renderer& mRenderer{ RegistryGetOrCreate<Renderer>("renderer") };

  Debug(std::string const& name, Object* pObject)
    : Actor(name, pObject)
  {
    Light* pLight{ ACS::CreateChild<Light>(this, "Light") };
    pLight->mpTransform->mPosition = { 0.f, 10.f, 0.f };
    pLight->mpLightPoint->mRadius = 10000.f;
    pLight->mpLightPoint->mColor = { 1.f, 1.f, 1.f, 1.f };
    pLight->mpLightPoint->mAttenuationLinear = 0.f;
    pLight->mpLightPoint->mAttenuationQuadratic = 0.002f;

    Terrain* pTerrain{ ACS::CreateChild<Terrain>(this, "Terrain") };
    pTerrain->mpTransform->mPosition = { 0.f, 1.f, 0.f };
    pTerrain->mpTransform->mScale = { 20.f, 1.f, 20.f };

    SphereMetal* pSphereMetal{ ACS::CreateChild<SphereMetal>(this, "SphereMetal") };
    pSphereMetal->mpTransform->mPosition = { -5.f, 5.f, 0.f };
    pSphereMetal->mpTransform->mScale = { 2.f, 2.f, 2.f };

    SphereRock* pSphereRock{ ACS::CreateChild<SphereRock>(this, "SphereRock") };
    pSphereRock->mpTransform->mPosition = { 5.f, 5.f, 0.f };
    pSphereRock->mpTransform->mScale = { 2.f, 2.f, 2.f };

    //for (u32 i{}; i < 8; i++)
    //  for (u32 j{}; j < 8; j++)
    //  {
    //    SphereMetal* pSphereMetal{ ACS::CreateChild<SphereMetal>(this, "SphereMetal_" + std::to_string(i) + "_" + std::to_string(j)) };
    //    pSphereMetal->mpTransform->mPosition = { (r32)i * 5, (r32)j * 5, 0.f };
    //    pSphereMetal->mpTransform->mScale = { 2.f, 2.f, 2.f };
    //
    //    SphereRock* pSphereRock{ ACS::CreateChild<SphereRock>(this, "SphereRock" + std::to_string(i) + "_" + std::to_string(j)) };
    //    pSphereRock->mpTransform->mPosition = { (r32)i * 5, (r32)j * 5, 50.f };
    //    pSphereRock->mpTransform->mScale = { 2.f, 2.f, 2.f };
    //  }
  }

  void OnGizmo() override
  {
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
    ACS::DebugActors(mRenderer);
    ACS::DebugComponents(mRenderer);

    ADB::DebugAssets();
    FBD::DebugDeferredTextures(mRenderer.mFrameBufferDeferred);
  }
};