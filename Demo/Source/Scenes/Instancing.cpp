#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/Instancing.h>

void SceneInstancing::OnEnable()
{
  CameraCreate(mCamera, { 0.f, 0.f, 0.f }, 45.f, 0.001f, 10000.f);
  ModelCreateInstanced(mModelSpaceShip, "C:\\Users\\Burmi\\Downloads\\Sandbox\\Models\\cube.fbx", sNumSpaceShips);
  RandomizeTransforms();
  RandomizeSteerings();
  BufferLayoutCreate(mBufferTransforms, sNumSpaceShips, 0);
  BufferLayoutCreate(mBufferSteerings, sNumSpaceShips, 1);
  BufferLayoutData(mBufferTransforms, mTransforms.data(), sNumSpaceShips);
  BufferLayoutData(mBufferSteerings, mSteerings.data(), sNumSpaceShips);
  ShaderCreateCompute(mShaderCompute, mShaderComputeSpaceShipUpdate);
  ShaderCreateRender(mShaderRender, mShaderRenderSpaceShipVertex, mShaderRenderSpaceShipFragment);
}
void SceneInstancing::OnDisable()
{
  ModelDestroyInstanced(mModelSpaceShip);
  BufferLayoutDestroy(mBufferTransforms);
  BufferLayoutDestroy(mBufferSteerings);
  ShaderDestroyRender(mShaderRender);
  ShaderDestroyCompute(mShaderCompute);
}
void SceneInstancing::OnUpdate(r32 timeDelta)
{
  CameraUpdateControllerInputOrbit(mCamera, mCameraController, timeDelta);

  //ShaderBind(mShaderCompute);
  //BufferLayoutBind(mBufferTransforms, 0);
  //BufferLayoutBind(mBufferSteerings, 1);
  //ShaderStorageBuffer(mShaderCompute, "TransformBuffer", 0);
  //ShaderStorageBuffer(mShaderCompute, "SteeringBuffer", 1);
  //ShaderUniformR32(mShaderCompute, "uTimeDelta", timeDelta);
  //ShaderExecuteCompute(mShaderCompute, 16, 1, 1);
}
void SceneInstancing::OnUpdateFixed(r32 timeDelta)
{
  CameraUpdateControllerPhysicsOrbit(mCamera, mCameraController);
}
void SceneInstancing::OnRender(r32 timeDelta) const
{
  ShaderBind(mShaderRender);
  ShaderStorageBuffer(mShaderRender, "TransformBuffer", 0);
  ShaderStorageBuffer(mShaderRender, "SteeringBuffer", 1);
  ShaderUniformR32M4(mShaderRender, "uProjection", mCamera.mProjection);
  ShaderUniformR32M4(mShaderRender, "uView", mCamera.mView);
  ModelRenderInstanced(mModelSpaceShip);
}
void SceneInstancing::OnGizmos(r32 timeDelta)
{
  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    Transform const& transform{ mTransforms[i] };
    Steering const& steering{ mSteerings[i] };

    GizmoLineBatchPushBox(transform.mPosition, transform.mScale, { 1.f, 0.f, 1.f, 1.f });
    GizmoLineBatchPushLine(transform.mPosition, transform.mPosition + steering.mVelocity, { 1.f, 1.f, 1.f, 1.f });
  }
}

void SceneInstancing::RandomizeTransforms()
{
  mTransforms.resize(sNumSpaceShips);

  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    r32v3 position{ glm::ballRand(500.f) };
    r32v3 rotation{ glm::sphericalRand(1.f) };
    r32v3 scale{ 1.f, 1.f, 1.f };
    r32m4 transform{ glm::identity<r32m4>() };

    //transform = glm::translate(transform, position);
    //transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
    //transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
    //transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });
    //transform = glm::scale(transform, scale);

    mTransforms[i] =
    {
      .mPosition { position },
      .mRotation { rotation },
      .mScale    { scale },
      .mTransform{ transform },
    };
  }
}

void SceneInstancing::RandomizeSteerings()
{
  mSteerings.resize(sNumSpaceShips);

  for (u32 i{}; i < sNumSpaceShips; i++)
  {
    mSteerings[i] =
    {
      .mAcceleration{ 0.f, 0.f, 0.f },
      .mVelocity    { 0.f, 0.f, 0.f },
    };
  }
}