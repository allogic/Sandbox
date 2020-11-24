#pragma once

#ifdef SANDBOX_ENGINE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif SANDBOX_ENGINE_IMPLEMENTATION

#include <Core.h>
#include <Types.h>
#include <Context.h>
#include <Utility.h>
#include <Registry.h>
#include <Events.h>
#include <Camera.h>
#include <Scene.h>
#include <Renderer.h>
#include <Importer.h>

#include <FrameBuffers/FrameBuffer.h>

#include <Layouts/VertexLayouts.h>
#include <Layouts/BufferLayouts.h>
#include <Layouts/TextureLayouts.h>
#include <Layouts/ShaderLayouts.h>
#include <Layouts/UniformLayouts.h>

#include <Materials/ComputeMaterials.h>
#include <Materials/RenderMaterials.h>

#include <Uniforms/GlobalUniform.h>
#include <Uniforms/ProjectionUniform.h>
#include <Uniforms/CameraUniform.h>
#include <Uniforms/LightUniform.h>

#include <Buffers/TransformBuffers.h>