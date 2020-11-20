#define STB_IMAGE_IMPLEMENTATION

#include <Api.h>

#include <Scenes/Game.h>

int main(int argc, char** argv)
{
  Context& context{ RegistryGetOrCreate<Context>("context") };

  ContextCreate(context, 1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  ContextSceneCreate(context, new SceneGame);
  ContextRun(context);
  ContextSceneDestroyAll(context);

  ContextDestroy(context);

  return 0;
}