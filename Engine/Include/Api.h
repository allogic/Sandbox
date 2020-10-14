#pragma once

#include <Types.h>
#include <Shader.h>
#include <Vertex.h>
#include <Material.h>
#include <Mesh.h>

#ifdef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#endif

#include <string>

void CreateShaderProgram();
void CreateVertexLayout();

#define SANDBOX_ENGINE_API
extern "C" SANDBOX_ENGINE_API void CreateWindow(GLFWwindow*& pWindow, u32 width, u32 height, std::string const& title);
extern "C" SANDBOX_ENGINE_API void CreateShader(Shader& shader, std::string const& vertexShaderSource, std::string const& fragmentShaderSource);
extern "C" SANDBOX_ENGINE_API void CreateModel(Mesh & mesh, Material & material, std::string const& fileName);
#undef SANDBOX_ENGINE_API