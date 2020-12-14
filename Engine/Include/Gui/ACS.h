#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <Renderer.h>

#include <Components/CameraComponents.h>

/*
* Gui interface for actors and componentes.
*/

static Object* spObjectSelected{};

namespace ACS
{
  static void DebugComponents(Renderer& renderer)
  {
    ImGui::Begin("Components");

    if (spObjectSelected)
    {
      RendererLineBatchPushLine(renderer, spObjectSelected->mpActor->mpTransform->mPosition, spObjectSelected->mpActor->mpTransform->mPosition + r32v3{ 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
      RendererLineBatchPushLine(renderer, spObjectSelected->mpActor->mpTransform->mPosition, spObjectSelected->mpActor->mpTransform->mPosition + r32v3{ 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });
      RendererLineBatchPushLine(renderer, spObjectSelected->mpActor->mpTransform->mPosition, spObjectSelected->mpActor->mpTransform->mPosition + r32v3{ 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f, 1.f });

      u64 componentIndex{};

      componentIndex = ACS::ComponentToIndex<Transform>();
      if (ACS::ContainsComponentBits(spObjectSelected->mMask, (s64)1 << componentIndex))
      {
        Transform* pTransform{ (Transform*)spObjectSelected->mComponents[componentIndex] };

        if (ImGui::TreeNodeEx("Transform"))
        {
          ImGui::SliderFloat3("Position", &pTransform->mPosition[0], -100.f, 100.f, "%.3f");
          ImGui::SliderFloat3("Rotation", &pTransform->mRotation[0], -180.f, 180.f, "%.3f");
          ImGui::SliderFloat3("Scale", &pTransform->mScale[0], -100.0f, 100.f, "%.3f");

          ImGui::TreePop();
        }
      }

      componentIndex = ACS::ComponentToIndex<Camera>();
      if (ACS::ContainsComponentBits(spObjectSelected->mMask, (s64)1 << componentIndex))
      {
        Camera* pCamera{ (Camera*)spObjectSelected->mComponents[componentIndex] };

        if (ImGui::TreeNodeEx("Camera"))
        {
          ImGui::Text("Aspect %.3f", pCamera->mAspect);
          ImGui::SliderFloat("Fov", &pCamera->mFovRad, -180.f, 180.f, "%.3f");
          ImGui::Text("Right %.3f %.3f %.3f", pCamera->mRight.x, pCamera->mRight.y, pCamera->mRight.z);
          ImGui::Text("Up %.3f %.3f %.3f", pCamera->mUp.x, pCamera->mUp.y, pCamera->mUp.z);
          ImGui::Text("Front %.3f %.3f %.3f", pCamera->mFront.x, pCamera->mFront.y, pCamera->mFront.z);
          ImGui::SliderFloat("Near", &pCamera->mNear, 0.001f, 1000000.f, "%.3f");

          ImGui::TreePop();
        }
      }

      componentIndex = ACS::ComponentToIndex<CameraControllerSpace>();
      if (ACS::ContainsComponentBits(spObjectSelected->mMask, (s64)1 << componentIndex))
      {
        CameraControllerSpace* pController{ (CameraControllerSpace*)spObjectSelected->mComponents[componentIndex] };

        if (ImGui::TreeNodeEx("Controller Space"))
        {
          ImGui::SliderFloat("Position Speed", &pController->mPositionAmount, 0.f, 100.f, "%.3f");
          ImGui::SliderFloat("Rotation Speed", &pController->mRotationAmount, 0.f, 100.f, "%.3f");
          ImGui::SliderFloat("Position Decay", &pController->mPositionDecay, 0.f, 100.f, "%.3f");
          ImGui::SliderFloat("Rotation Decay", &pController->mRotationDecay, 0.f, 100.f, "%.3f");
          ImGui::Text("Position Accel %.3f %.3f %.3f", pController->mPositionAccel.x, pController->mPositionAccel.y, pController->mPositionAccel.z);
          ImGui::Text("Rotation Accel %.3f %.3f %.3f", pController->mRotationAccel.x, pController->mRotationAccel.y, pController->mRotationAccel.z);

          ImGui::TreePop();
        }
      }

      componentIndex = ACS::ComponentToIndex<LightPoint>();
      if (ACS::ContainsComponentBits(spObjectSelected->mMask, (s64)1 << componentIndex))
      {
        LightPoint* pLightPoint{ (LightPoint*)spObjectSelected->mComponents[componentIndex] };

        if (ImGui::TreeNodeEx("Light Point"))
        {
          ImGui::SliderFloat("Radius", &pLightPoint->mRadius, 0.f, 10000.f, "%.3f");
          ImGui::ColorEdit4("Color", &pLightPoint->mColor[0], 0);
          ImGui::SliderFloat("Attenuation Linear", &pLightPoint->mAttenuationLinear, 0.001f, 10.f, "%.3f");
          ImGui::SliderFloat("Attenuation Quadratic", &pLightPoint->mAttenuationQuadratic, 0.001f, 10.f, "%.3f");

          ImGui::TreePop();
        }
      }
    }

    ImGui::End();
  }

  static void DebugActorChildsRecursive(Renderer& renderer, Actor* pActor)
  {
    if (ImGui::TreeNodeEx(pActor, 0, "%s", pActor->mName.c_str()))
    {
      if (ImGui::Button("+")) CreateChild<Actor>(pActor, std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
      ImGui::SameLine();
      if (ImGui::Button("-")) 
      ImGui::SameLine();

      if (ImGui::Button("Ungroup")) Ungroup(pActor);
      ImGui::SameLine();
      if (ImGui::Button("Ungroup All")) UngroupRecursive(pActor);
      ImGui::SameLine();

      if (ImGui::Button("Destroy")) Destroy(pActor);
      ImGui::SameLine();
      if (ImGui::Button("Destroy All")) DestroyRecursive(pActor);
      ImGui::SameLine();

      if (ImGui::Button(">")) spObjectSelected = pActor->mpObject;

      for (auto const& pChild : pActor->mChilds)
      {
        DebugActorChildsRecursive(renderer, pChild);
      }

      ImGui::TreePop();
    }
  }
  static void DebugActors(Renderer& renderer)
  {
    ImGui::Begin("Actors");

    for (const auto& [actorName, pObject] : sObjectRegistry)
    {
      if (!pObject->mpActor->mpParent)
      {
        DebugActorChildsRecursive(renderer, pObject->mpActor);
      }
    }

    ImGui::End();
}
}