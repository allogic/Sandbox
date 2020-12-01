#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/Player.h>
#include <Actors/Patrol.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ACS::Create<Player>("Player");

  for (u32 i{}; i < 32; i++)
  {
    Patrol& patrol{ ACS::Create<Patrol>("Patrol_" + std::to_string(i), i) };

    r32v2 randomPosition{ glm::diskRand(100.f) };

    patrol.mTransform.mPosition = { randomPosition.x, 0.f, randomPosition.y };
  }
  
  ContextRun(context);

  ContextDestroy(context);

  return 0;
}