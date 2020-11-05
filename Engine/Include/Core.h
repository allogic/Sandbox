#pragma once

#define SANDBOX_ROOT_PATH "C:\\Users\\Michael\\Downloads\\Sandbox\\"

#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <cstring>
#include <fstream>
#include <functional>
#include <cstdio>
#include <limits>
#include <utility>
#include <cstdint>

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