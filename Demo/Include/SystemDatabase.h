#pragma once

#include <Api.h>

/*
* Map layouts.
*/

struct Body;

struct Planet
{
  r32   mRadius  {};
  r32v3 mPosition{};
  r32v3 mRotation{};
  r32v3 mScale   {};
};
struct System
{
  Planet              mSun     {};
  std::vector<Planet> mPlanets {};
  //r32v3               mPosition{};
  //r32v3               mRotation{};
  //r32v3               mScale   {};
};

static std::map<std::string, System> sSystemRegistry{};

/*
* Map management.
*/

static System& SystemDatabaseGet(std::string const& systemName)
{
  auto const systemIt{ sSystemRegistry.find(systemName) };

  if (systemIt == sSystemRegistry.end())
  {
    System system{};

    static std::random_device randomDevice{};
    static std::default_random_engine randomEngine{ randomDevice() };

    static std::uniform_real_distribution<r32> normalDist{ 0.f, 1.f };
    static std::uniform_int_distribution<u32> planetDist{ 1, 100 };

    r32 const sunRadius{ normalDist(randomEngine) * 100.f + 50.f };
    u32 const numPlanets{ planetDist(randomEngine) };

    system.mSun = { sunRadius, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f } };

    for (u32 i{}, j{}; i < 1024; i++)
    {
      if (j == numPlanets) break;

      r32 const planetRadius{ normalDist(randomEngine) * 1000.f + 500.f };
      r32v3 const planetPosition{ planetDist(randomEngine) * 100.f, planetDist(randomEngine) * 100.f, planetDist(randomEngine) * 100.f };
      r32v3 const planetRotation{ planetDist(randomEngine) * 360.f - 180.f, planetDist(randomEngine) * 360.f - 180.f, planetDist(randomEngine) * 360.f - 180.f };
      r32v3 const planetScale{ planetRadius / 2.f, planetRadius / 2.f, planetRadius / 2.f };

      for (const auto& planet : system.mPlanets)
      {

      }

      system.mPlanets.emplace_back(Planet{ planetRadius, planetPosition, planetRotation, planetScale });

      j++;
    }

    auto const [insertIt, _]{ sSystemRegistry.emplace(systemName, system) };
    return insertIt->second;
  }

  return systemIt->second;
}