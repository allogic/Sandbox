#pragma once

#include <Core.h>
#include <Types.h>
#include <Context.h>
#include <Utility.h>
#include <Registry.h>
#include <Events.h>
#include <Camera.h>
#include <Scene.h>
#include <FrameBuffer.h>
#include <Renderer.h>

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

#include <Buffers/Transform.h>

/*
* Model management.
*/

extern "C" void ModelCreate(ModelLambert& model, std::string const& fileName);
extern "C" void ModelRender(ModelLambert const& model);
extern "C" void ModelRenderInstanced(ModelLambert const& model, u32 numInstances);
extern "C" void ModelDestroy(ModelLambert const& model);