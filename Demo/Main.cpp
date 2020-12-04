#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/DebugActor.h>
#include <Actors/PlayerActor.h>
#include <Actors/EntitySpawnerActor.h>

#include <Actors/Ambient/AsteroidFieldActor.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ACS::Create<Player>("Player");
  ACS::Create<Debug>("Debug");
  ACS::Create<AsteroidField>("AsteroidField", (u32)(1024 * 32));
  ACS::Create<EntitySpawner>("EntitySpawner", 1.f / 1);

  ContextRun(context);

  ContextDestroy(context);

  return 0;
}