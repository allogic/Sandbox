#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/Enemy.h>
#include <Actors/Player.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ACS::Create<Player>("Player");

  //for (u32 i{}; i < 32; i++)
  //{
  //  ACS::Create<Enemy>("Enemy_" + std::to_string(i), "CruiserBerlin");
  //}
  
  ContextRun(context);

  ContextDestroy(context);

  return 0;
}