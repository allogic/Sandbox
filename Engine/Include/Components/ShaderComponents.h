#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Shader sources.
*/

struct ShaderPaths
{
  std::string mVertex  {};
  std::string mFragment{};
  std::string mCompute {};
};

/*
* Shader components.
*/

enum ShaderLayoutType : u32
{
  eShaderLayoutLambert,
  eShaderLayoutLambertInstanced,
  eShaderLayoutCompute,
  eShaderLayoutGizmo,
  eShaderLayoutScreen,
};

template<ShaderLayoutType Layout>
struct ShaderLayout : Component
{
  constexpr static u32 sLayout{ Layout };

  u32 mProgId{};
  u32 mVertId{};
  u32 mFragId{};
  u32 mCompId{};
};

using ShaderLambert          = ShaderLayout<eShaderLayoutLambert>;
using ShaderLambertInstanced = ShaderLayout<eShaderLayoutLambertInstanced>;
using ShaderCompute          = ShaderLayout<eShaderLayoutCompute>;
using ShaderGizmo            = ShaderLayout<eShaderLayoutGizmo>;
using ShaderScreen           = ShaderLayout<eShaderLayoutScreen>;

/*
* Shader utilities.
*/

static u32 ShaderLayoutLoadBinary(std::vector<s8>& shaderBytes, std::string const& fileName)
{
  std::ifstream file{ fileName, std::ios::binary };

  if (!file.is_open()) return 1;

  shaderBytes.clear();

  shaderBytes =
  {
    std::istreambuf_iterator<s8>{ file },
    std::istreambuf_iterator<s8>{}
  };

  file.close();

  return file.is_open() ? 1 : 0;
}
static u32 ShaderLayoutCheckCompileStatus(u32 shaderId, std::string& log)
{
  s32 compileStatus{};
  s32 infoLogLength{};

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

if (!compileStatus)
{
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

  log.clear();
  log.resize(infoLogLength);

  glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, log.data());

  return 0;
}

return 1;
}
static u32 ShaderLayoutCheckLinkStatus(u32 programId, std::string& log)
{
  s32 linkStatus{};
  s32 infoLogLength{};

  glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

  if (!linkStatus)
  {
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

    log.clear();
    log.resize(infoLogLength);

    glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, log.data());

    return 0;
  }

  return 1;
}

/*
* Shader management.
*/

template<typename ShaderLayout> void ShaderLayoutDestroy(ShaderLayout const& shaderLayout)
{
  if (shaderLayout.mVertId) glDeleteShader(shaderLayout.mVertId);
  if (shaderLayout.mFragId) glDeleteShader(shaderLayout.mFragId);
  if (shaderLayout.mCompId) glDeleteShader(shaderLayout.mCompId);

  if (shaderLayout.mProgId) glDeleteProgram(shaderLayout.mProgId);
}
template<typename ShaderLayout> void ShaderLayoutCreate(ShaderLayout& shaderLayout, ShaderPaths const& shaderPaths)
{
  std::string log{};

  switch (ShaderLayout::sLayout)
  {
    case eShaderLayoutLambert:
    case eShaderLayoutLambertInstanced:
    case eShaderLayoutGizmo:
    case eShaderLayoutScreen:
    {
      shaderLayout.mVertId = glCreateShader(GL_VERTEX_SHADER);
      shaderLayout.mFragId = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    }
    case eShaderLayoutCompute:
    {
      shaderLayout.mCompId = glCreateShader(GL_COMPUTE_SHADER);
      break;
    }
  }

  std::vector<s8> shaderBytes{};

  switch (ShaderLayout::sLayout)
  {
    case eShaderLayoutLambert:
    case eShaderLayoutLambertInstanced:
    case eShaderLayoutGizmo:
    case eShaderLayoutScreen:
    {
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mVertex);
      glShaderBinary(1, &shaderLayout.mVertId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mVertId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mVertId, log)) std::printf("%s\n", log.data());
      
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mFragment);
      glShaderBinary(1, &shaderLayout.mFragId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mFragId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mFragId, log)) std::printf("%s\n", log.data());
      break;
    }
    case eShaderLayoutCompute:
    {
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mCompute);
      glShaderBinary(1, &shaderLayout.mCompId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mCompId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mCompId, log)) std::printf("%s\n", log.data());
      break;
    }
  }

  shaderLayout.mProgId = glCreateProgram();

  switch (ShaderLayout::sLayout)
  {
    case eShaderLayoutLambert:
    case eShaderLayoutLambertInstanced:
    case eShaderLayoutGizmo:
    case eShaderLayoutScreen:
    {
      glAttachShader(shaderLayout.mProgId, shaderLayout.mVertId);
      glAttachShader(shaderLayout.mProgId, shaderLayout.mFragId);
      break;
    }
    case eShaderLayoutCompute:
    {
      glAttachShader(shaderLayout.mProgId, shaderLayout.mCompId);
      break;
    }
  }

  glLinkProgram(shaderLayout.mProgId);

  if (!ShaderLayoutCheckLinkStatus(shaderLayout.mProgId, log)) std::printf("%s\n", log.data());

  switch (ShaderLayout::sLayout)
  {
    case eShaderLayoutLambert:
    case eShaderLayoutLambertInstanced:
    case eShaderLayoutGizmo:
    case eShaderLayoutScreen:
    {
      glDetachShader(shaderLayout.mProgId, shaderLayout.mVertId);
      glDetachShader(shaderLayout.mProgId, shaderLayout.mFragId);
      break;
    }
    case eShaderLayoutCompute:
    {
      glDetachShader(shaderLayout.mProgId, shaderLayout.mCompId);
      break;
    }
  }
}
template<typename ShaderLayout> void ShaderLayoutBind(ShaderLayout const& shaderLayout)
{
  glUseProgram(shaderLayout.mProgId);
}
static                          void ShaderLayoutUnbind()
{
  glUseProgram(0);
}
template<typename ShaderLayout> void ShaderLayoutGetBindings(ShaderLayout const& shaderLayout, u32 bindingType, std::map<std::string, u32>& bindings)
{
  u32 numResources{};
  u32 numResourcesCharsMax{};

  std::string resourceNameBuffer{};
  resourceNameBuffer.resize(128);

  glGetProgramInterfaceiv(shaderLayout.mProgId, bindingType, GL_ACTIVE_RESOURCES, (s32*)(&numResources));

  for (u32 i{}; i < numResources; i++)
  {
    glGetProgramResourceName(shaderLayout.mProgId, bindingType, i, 128, (s32*)(&numResourcesCharsMax), resourceNameBuffer.data());

    std::string resourceName{};
    resourceName.resize(numResourcesCharsMax);
    resourceName.assign(resourceNameBuffer.begin(), resourceNameBuffer.begin() + numResourcesCharsMax);

    u32 resourceId{ glGetProgramResourceIndex(shaderLayout.mProgId, bindingType, resourceName.data()) };

    bindings.emplace(resourceName, resourceId);
  }
}
template<typename ShaderLayout> void ShaderLayoutCompute(ShaderLayout const& shaderLayout, u32 numThreadsX, u32 numThreadsY, u32 numThreadsZ)
{
  glDispatchCompute(numThreadsX, numThreadsY, numThreadsZ);
}

/*
* Shader uniform bindings.
*/

template<typename ShaderLayout> void ShaderLayoutUniformS32(ShaderLayout const& shaderLayout, std::string const& uniformName, s32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data()) };
  glUniform1i(uniformIndex, value);
}
template<typename ShaderLayout> void ShaderLayoutUniformU32(ShaderLayout const& shaderLayout, std::string const& uniformName, u32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data()) };
  glUniform1ui(uniformIndex, value);
}
template<typename ShaderLayout> void ShaderLayoutUniformR32(ShaderLayout const& shaderLayout, std::string const& uniformName, r32 value)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data()) };
  glUniform1f(uniformIndex, value);
}
template<typename ShaderLayout> void ShaderLayoutUniformR32V3(ShaderLayout const& shaderLayout, std::string const& uniformName, r32v3 const& vector)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data()) };
  glUniform3fv(uniformIndex, 1, &vector[0]);
}
template<typename ShaderLayout> void ShaderLayoutUniformR32M4(ShaderLayout const& shaderLayout, std::string const& uniformName, r32m4 const& matrix)
{
  u32 uniformIndex{ (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data()) };
  glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, &matrix[0][0]);
}