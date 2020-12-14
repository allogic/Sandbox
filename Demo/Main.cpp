#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/DebugActor.h>
#include <Actors/PlayerActor.h>

int main(int argc, char** argv)
{
  Window& window{ RegistryGetOrCreate<Window>("window") };

  WindowCreate(window, 1280, 720, "Sandbox");
  WindowRegisterDebugHandler();

  Player* pPlayer{ ACS::Create<Player>("Player") };
  pPlayer->mpTransform->mPosition = { 0.f, 5.f, 10.f };
  Debug* pDebug{ ACS::Create<Debug>("Debug") };

  WindowRun(window);

  WindowDestroy(window);

  return 0;
}