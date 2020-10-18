#pragma once

#include <Core.h>
#include <Fwdecls.h>

using s32 = int;
using u32 = int unsigned;

using r32 = float;
using r64 = double;

using r32v2 = glm::fvec2;
using r32v3 = glm::fvec3;
using r32v4 = glm::fvec4;

using r32m3 = glm::fmat3;
using r32m4 = glm::fmat4;

using WindowCloseClb = void(*)(GLFWwindow*);
using MouseMoveClb = void(*)(GLFWwindow*, r64, r64);
using MouseButtonClb = void(*)(GLFWwindow*, s32, s32, s32);
using KeyboardClb = void(*)(GLFWwindow*, s32, s32, s32, s32);