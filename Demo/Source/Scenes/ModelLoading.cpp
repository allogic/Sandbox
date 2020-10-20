#define SANDBOX_ENGINE_INCLUDE_DEPENDENCIES
#include <Api.h>
#undef SANDBOX_ENGINE_INCLUDE_DEPENDENCIES

#include <Scenes/ModelLoading.h>

std::string const sVertexShaderSource
{
R"glsl(
#version 460 core

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec3 lNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 mTransform;

out vec3 position;
out vec3 normal;

void main()
{
  position = vec3(mTransform * vec4(lPosition, 1.0f));
  normal = vec3(mTransform * vec4(lNormal, 1.0f));

  gl_Position = uProjection * uView * vec4(position, 1.0f);
}
)glsl"
};

std::string const sFragmentShaderSource
{
R"glsl(
#version 460 core

in vec3 position;
in vec3 normal;

out vec4 color;

void main()
{
  color = vec4(normal, 1.0f);
}
)glsl"
};

void SceneModelLoading::OnEnable(r32 aspect)
{
  CameraCreate(mCamera, { 0.f, 0.f, -50.f }, 45.f, aspect, 0.001f, 1000.f);
  ModelCreate(mModel, "C:\\Users\\Michael\\Downloads\\Sandbox\\Models\\teapot.fbx");
  ShaderCreate(mShader, sVertexShaderSource, sFragmentShaderSource);
}

void SceneModelLoading::OnDisable()
{
  ModelDestroy(mModel);
  ShaderDestroy(mShader);
}

void SceneModelLoading::OnUpdate(r32 timeDelta)
{
  CameraUpdateController(mCamera, mCameraController, timeDelta);
}

void SceneModelLoading::OnRender() const
{
  glClearColor(0.f, 0.2f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  BindShader(mShader);
  ApplyUniformMat4(mShader, "uProjection", mCamera.mProjection);
  ApplyUniformMat4(mShader, "uView", mCamera.mView);
  ApplyUniformMat4(mShader, "mTransform", mModel.mTransform);
  BindModel(mModel);
}