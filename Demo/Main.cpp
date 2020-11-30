#define SANDBOX_ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/Enemy.h>
#include <Actors/Player.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ACS::Create<Player>("Player", "FighterAtlas");

  for (u32 i{}; i < 32; i++)
  {
    Enemy& enemy{ ACS::Create<Enemy>("Enemy_" + std::to_string(i), "FighterAtlas") };
    enemy.mTransform.mPosition = { (r32)(i - 16) * 16.f, 0.f, -20.f };
  }
  
  ContextRun(context);

  ContextDestroy(context);

  return 0;
}