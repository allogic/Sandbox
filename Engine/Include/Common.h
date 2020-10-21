#pragma once

#include <Types.h>
#include <Core.h>
#include <Fwdecls.h>

using Materials = std::vector<Material>;
using Meshes    = std::vector<Mesh>;
using Vertices  = std::vector<Vertex>;
using Indices   = std::vector<u32>;

struct SANDBOX_ENGINE_API Material
{
  
};

struct SANDBOX_ENGINE_API Vertex
{
  r32v3 mPosition{};
  r32v3 mNormal  {};
  r32v4 mColor   {};
};

struct SANDBOX_ENGINE_API Mesh
{
  u32      mVao        {};
  u32      mVbo        {};
  u32      mEbo        {};
  Vertices mVertices   {};
  Indices  mIndices    {};
};

struct SANDBOX_ENGINE_API Model
{
  Materials mMaterials{};
  Meshes    mMeshes   {};
  r32m4     mTransform{};
};

struct SANDBOX_ENGINE_API Camera
{
  r32v3 mPosition  {};
  r32   mFovRad    {};
  r32v3 mRight     {};
  r32v3 mUp        {};
  r32v3 mFront     {};
  r32v3 mLocalRight{};
  r32v3 mLocalUp   {};
  r32v3 mLocalFront{};
  r32   mNear      {};
  r32   mFar       {};
  r32m4 mProjection{};
  r32m4 mView      {};
};

struct SANDBOX_ENGINE_API CameraControllerOrbit
{
  r32   mPositionSpeed   { 1000.f };
  r32   mRotationSpeed   { 3.f };
  u32   mLockDrag        {};
  r32v2 mRotationDrag    {};
  r32   mRotationDecay   { 1000.f };
  r32   mRotationDeadzone{ 0.001f };
  r32v2 mRotationVelocity{};
};

struct SANDBOX_ENGINE_API Texture
{
  u32 mTid{};
};

struct SANDBOX_ENGINE_API Shader
{
  u32 mPid{};
  u32 mVid{};
  u32 mFid{};
};

struct SANDBOX_ENGINE_API Scene
{
  Camera mCamera{};

  virtual ~Scene() = default;

  virtual void OnEnable() = 0;
  virtual void OnDisable() = 0;
  virtual void OnUpdate(r32 timeDelta) = 0;
  virtual void OnRender() const = 0;
};