#pragma once

#ifdef SANDBOX_ENGINE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif SANDBOX_ENGINE_IMPLEMENTATION

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <ADB.h>
#include <Window.h>
#include <Utility.h>
#include <Registry.h>
#include <Events.h>
#include <Renderer.h>
#include <Importer.h>
#include <FrameBufferDeferred.h>

#include <Gui/ACS.h>
#include <Gui/ADB.h>
#include <Gui/FrameBufferDeferred.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/ShaderLayouts.h>
#include <Layouts/TextureLayouts.h>
#include <Layouts/BufferLayouts.h>
#include <Layouts/UniformLayouts.h>

#include <Components/LightComponents.h>
#include <Components/CameraComponents.h>
#include <Components/RenderComponents.h>

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>