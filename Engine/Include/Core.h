#pragma once

#define SANDBOX_ENGINE_ROOT_PATH "C:\\Users\\Michael\\Downloads\\Sandbox\\"

#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <variant>
#include <string>
#include <cstring>
#include <fstream>
#include <functional>
#include <cstdio>
#include <limits>
#include <utility>
#include <cstdint>
#include <chrono>
#include <cassert>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#define _STR(VAL) #VAL
#define STR(VAL) _STR(VAL)

#define SANDBOX_ENGINE_MEASURE_BEGIN(NAME)                                                                    \
auto NAME##Start = std::chrono::high_resolution_clock::now();

#define SANDBOX_ENGINE_MEASURE_END(NAME)                                                                      \
auto NAME##End = std::chrono::high_resolution_clock::now();                                                   \
auto NAME##Duration = std::chrono::duration_cast<std::chrono::milliseconds>(NAME##End - NAME##Start).count(); \
std::printf(STR(NAME) ": %lldms\n", NAME##Duration);