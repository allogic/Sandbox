#pragma once

#ifdef SANDBOX_ENGINE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif SANDBOX_ENGINE_IMPLEMENTATION

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <Context.h>
#include <Utility.h>
#include <Registry.h>
#include <Events.h>
#include <Renderer.h>
#include <Importer.h>

#include <Buffers/TransformBuffers.h>

#include <Components/VertexComponents.h>
#include <Components/BufferComponents.h>
#include <Components/TextureComponents.h>
#include <Components/ShaderComponents.h>
#include <Components/UniformComponents.h>
#include <Components/LightComponents.h>
#include <Components/TransformComponents.h>
#include <Components/CameraComponents.h>
#include <Components/RenderComponents.h>

#include <FrameBuffers/FrameBufferDeferred.h>

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/LambertUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>