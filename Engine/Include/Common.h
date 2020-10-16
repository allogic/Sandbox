#pragma once

#include <Types.h>
#include <Core.h>

struct GLFWwindow;
struct Material;
struct Mesh;

using Materials = std::vector<Material>;
using Meshes    = std::vector<Mesh>;

struct SANDBOX_ENGINE_API Window
{
  GLFWwindow* mpHdl{};
};

struct SANDBOX_ENGINE_API Material
{
  
};

struct SANDBOX_ENGINE_API Mesh
{
  u32 mVao{};
  u32 mVbo{};
  u32 mEbo{};
};

struct SANDBOX_ENGINE_API Model
{
  Materials mMaterials {};
  Meshes    mMeshes    {};
  r32m4     mProjection{};
};

struct SANDBOX_ENGINE_API Camera
{
  r32v3 mPosition  {};
  r32v3 mUp        {};
  r32m4 mProjection{};
  r32   mFov       {};
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