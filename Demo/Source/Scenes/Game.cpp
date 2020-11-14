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
  mShipManager.Update(timeDelta);
}
void SceneGame::OnUpdateFixed(r32 timeDelta)
{
  mPlayerManager.UpdatePhysics(timeDelta);
  mShipManager.UpdatePhysics(timeDelta);
}
void SceneGame::OnRender(r32 timeDelta)
{
  //mShipManager.Render();
}
void SceneGame::OnGizmos(r32 timeDelta)
{
  mShipManager.Debug();

  u32 size{ 32 };

  for (u32 i{}; i <= size; i++)
    for (u32 j{}; j <= size; j++)
    {
      r32v3 spacing{ 100.f, 100.f, 100.f };
      r32v3 half{ size / 2, 0.f, size / 2 };
      r32v3 startP0{ i, 0, 0 };
      r32v3 startP1{ 0, 0, j };
      r32v3 endP0{ i, 0, size };
      r32v3 endP1{ size, 0, j };
      r32v4 color{ 1.f, 1.f, 0.f, 1.f };

      startP0 *= spacing;
      startP1 *= spacing;
      startP0 += -half * spacing;
      startP1 += -half * spacing;

      endP0 *= spacing;
      endP1 *= spacing;
      endP0 += -half * spacing;
      endP1 += -half * spacing;

      GizmoLineBatchPushLine(startP0, endP0, color);
      GizmoLineBatchPushLine(startP1, endP1, color);
    }
}