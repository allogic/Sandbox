#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Light components.
*/

enum LightFormType
{
  eLightFormDirectional,
  eLightFormPoint,
};

template<u32 Type>
struct LightLayout : Component
{
  constexpr static u32 sType{ Type };
};

using LightDirctional = LightLayout<eLightFormDirectional>;
using LightPoint      = LightLayout<eLightFormPoint>;