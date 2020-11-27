#include <Scenes/Game.h>

void SceneGame::OnEnable()
{

}
void SceneGame::OnDisable()
{

}
void SceneGame::OnUpdate(r32 timeDelta)
{
  mPlayerManager.Update(timeDelta);
  //mShipManager.Update(timeDelta);
  mBackgroundManager.Update(timeDelta);
  mMapManager.Update(timeDelta);
}
void SceneGame::OnUpdateFixed(r32 timeDelta)
{
  mPlayerManager.UpdatePhysics(timeDelta);
  //mShipManager.UpdatePhysics(timeDelta);
  mBackgroundManager.UpdatePhysics(timeDelta);
  mMapManager.UpdatePhysics(timeDelta);
}
void SceneGame::OnRender()
{
  mPlayerManager.Render();
  mShipManager.Render();
  //mBackgroundManager.Render();
  mMapManager.Render();
}
void SceneGame::OnGizmos(r32 timeDelta)
{
  //mPlayerManager.Debug();
  //mShipManager.Debug();
  //mBackgroundManager.Debug();
  //mMapManager.Debug();
}