#pragma once

#include <Types.h>
#include <Core.h>
#include <Common.h>

#ifdef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif

extern "C" SANDBOX_ENGINE_API void CreateWindow(Window& window, u32 width, u32 height, std::string const& title);
extern "C" SANDBOX_ENGINE_API void CreateShader(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void CreateModel(Model& model, std::string const& fileName);