#define STB_IMAGE_IMPLEMENTATION

#include <Api.h>

#include <Scenes/Game.h>

int main(int argc, char** argv)
{
  ContextCreate(1280, 720, "Sandbox");
  ContextRegisterDebugHandler();

  SceneCreate(new SceneGame);

  ContextRun();

  SceneDestroyAll();

  ContextDestroy();

  return 0;
}