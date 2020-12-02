#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/Player.h>
#include <Actors/EntitySpawner.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ACS::Create<Player>("Player");
  ACS::Create<EntitySpawner>("EntitySpawner", 1.f / 60);

  ContextRun(context);

  ContextDestroy(context);

  return 0;
}