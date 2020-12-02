#pragma once

#include <Core.h>
#include <Types.h>
#include <Transform.h>

/*
* Type deduction utilities.
*/

template<typename T>
struct Identity
{
  using Type = T;
  using Ptr  = T*;
  using CPtr = T const*;
};

/*
* Forward decls.
*/

struct Actor;

/*
* Actor component system.
*/

struct Component
{
  
};
struct Object
{
  Actor*                     mpActor     {};
  u64                        mMask       {};
  std::array<Component*, 64> mComponents {};
  u32                        mDestroyFlag{};
};
struct Actor
{
  Actor*              mpParent  {};
  std::vector<Actor*> mChilds   {};

  Transform           mTransform{};

  std::string         mName;
  Object*             mpObject;

  Actor(std::string const& name, Object* pObject)
    : mName{ name }
    , mpObject{ pObject } {}
  
  virtual void OnEnable() {};
  virtual void OnDisable() {};
  virtual void OnUpdate(r32 time, r32 timeDelta) {};
  virtual void OnUpdateFixed(r32 time, r32 timeDelta) {}
  virtual void OnGizmo() {}
  virtual void OnImGui() {}
};

static std::map<std::string, Object*> sObjectRegistry  {};
static u64                            sUniqueTypeCount {};
static std::map<u64, u64>             sIdentityRegistry{};

namespace ACS
{
  /*
  * Helper utilities.
  */
  
  template<typename C>
  requires std::is_base_of_v<Component, C>
  s64 ComponentToIndex()
  {
    auto const identityIt{ sIdentityRegistry.find(typeid(C).hash_code()) };
  
    if (identityIt == sIdentityRegistry.end())
    {
      auto const [insertIt, _]{ sIdentityRegistry.emplace(typeid(C).hash_code(), sUniqueTypeCount++) };
      return insertIt->second;
    }
  
    return identityIt->second;
  }
  
  constexpr static u32 ContainsComponentBits(s64 entityMask, s64 componentMask)
  {
    return (~(entityMask ^ componentMask) & componentMask) == componentMask;
  }

  static void DebugTreeChildsRecursive(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      if (ImGui::TreeNodeEx(pChild, 0, "%s", pChild->mName.c_str()))
      {
        DebugTreeChildsRecursive(pChild);

        ImGui::TreePop();
      }
    }
  }
  static void DebugTreeActors()
  {
    ImGui::Begin("ACS");

    for (const auto& [_, pObject] : sObjectRegistry)
    {
      if (!pObject->mpActor->mpParent)
      {
        if (ImGui::TreeNodeEx(pObject->mpActor, 0, "%s", pObject->mpActor->mName.c_str()))
        {
          DebugTreeChildsRecursive(pObject->mpActor);

          /*
          for (const auto& [hash, pComponent] : components)
          {
            if (ImGui::TreeNodeEx(pComponent, 0, "%s", pComponent->name))
            {
              if (hash == Register<ACS::Components::TTransform>())
              {
                auto pTransform = (ACS::Components::TTransform*)pComponent;

                ImGui::InputFloat3("Position", &pTransform->position[0]);
                ImGui::InputFloat3("Rotation", &pTransform->rotation[0]);
                ImGui::InputFloat3("Scale", &pTransform->scale[0]);
                ImGui::InputFloat3("Right", &pTransform->right[0]);
                ImGui::InputFloat3("Up", &pTransform->up[0]);
                ImGui::InputFloat3("Forward", &pTransform->forward[0]);
                ImGui::InputFloat3("Local Right", &pTransform->localRight[0]);
                ImGui::InputFloat3("Local Up", &pTransform->localUp[0]);
                ImGui::InputFloat3("Local Forward", &pTransform->localForward[0]);
              }
              else if (hash == Register<ACS::Components::TCamera>())
              {
                auto pCamera = (ACS::Components::TCamera*)pComponent;

                ImGui::Checkbox("Projection", (b8*)(&pCamera->projection));

              }
              else if (hash == Register<ACS::Components::TMesh>())
              {
                auto pMesh = (ACS::Components::TMesh*)pComponent;

                ImGui::LabelText("Vertex Count", "%d", pMesh->pVertexLayout->vertexCount);
                ImGui::LabelText("Triangle Count", "%d", pMesh->pVertexLayout->indexCount / 3);
              }

              ImGui::TreePop();
            }
          }
          */

          ImGui::TreePop();
        }
      }
    }

    ImGui::End();
  }

  /*
  * Actor management.
  */
  
  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* Create(std::string const& name, Args&& ... args)
  {
    auto const objectIt{ sObjectRegistry.find(name) };
  
    if (objectIt == sObjectRegistry.end())
    {
      Object* pObject{ new Object };
  
      auto const [insertIt, _] { sObjectRegistry.emplace(name, pObject) };
  
      // create actor at least in order to maintain sub-component order initialization 
      pObject->mpActor = new A{ name, pObject, std::forward<Args>(args) ... };
  
      return (A*)insertIt->second->mpActor;
    }
  
    return (A*)objectIt->second->mpActor;
  }
  
  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* CreateChild(Actor* pActor, std::string const& actorName, Args&& ... args)
  {
    A* pChild{ Create<A>(actorName, std::forward<Args>(args) ...) };
    pChild->mpParent = pActor;

    pActor->mChilds.emplace_back(pChild);

    return pChild;
  }

  static void UngroupRecursive(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      UngroupRecursive(pChild);
    }

    pActor->mpParent = nullptr;
    pActor->mChilds.clear();
  }
  static void Ungroup(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      pChild->mpParent = nullptr;
    }

    pActor->mChilds.clear();
  }

  static void DestroyRecursive(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      DestroyRecursive(pChild);
    }

    Ungroup(pActor);

    pActor->mpObject->mDestroyFlag = 1;
  }
  static void Destroy(Actor* pActor)
  {
    Ungroup(pActor);

    pActor->mpObject->mDestroyFlag = 1;
  }

  /*
  * Component management.
  */

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C* Attach(Actor* pActor, Args&& ... args)
  {
    s64 const componentIndex{ ComponentToIndex<C>() };
    s64 const componentMask{ (s64)1 << componentIndex };
  
    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {  
      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
    else
    {  
      pActor->mpObject->mMask |= componentMask;
      pActor->mpObject->mComponents[componentIndex] = new C{ std::forward<Args>(args) ... };
  
      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
  }
  
  /*
  * Object dispatching.
  */

  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void Dispatch(std::function<void(Actor*)>&& predicate)
  {
    for (auto const& [actorName, pObject] : sObjectRegistry)
    {
      if (!pObject->mDestroyFlag)
      {
        predicate(pObject->mpActor);
      }
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchFor(std::function<void(Actor*, typename Identity<Comps>::Ptr ...)>&& predicate)
  {
    s64 const componentMask{ (((s64)1 << ComponentToIndex<typename Identity<Comps>::Type>()) | ... | (s64)0) };

    for (auto const& [actorName, pObject] : sObjectRegistry)
    {
      if (!pObject->mDestroyFlag)
      {
        if (ContainsComponentBits(pObject->mMask, componentMask))
        {
          predicate
          (
            pObject->mpActor,
            (typename Identity<Comps>::Ptr)(pObject->mComponents[ComponentToIndex<typename Identity<Comps>::Type>()])
            ...
          );
        }
      }
    }
  }

  /*
  * Internal state management.
  */

  static void Update()
  {
    for (auto objectIt{ sObjectRegistry.begin() }; objectIt != sObjectRegistry.end();)
    {
      if (objectIt->second->mDestroyFlag)
      {
        for (u32 i{}; i < 64; i++)
        {
          if (ContainsComponentBits(objectIt->second->mMask, (s64)1 << i))
          {
            delete objectIt->second->mComponents[i];
            objectIt->second->mComponents[i] = nullptr;
          }
        }

        delete objectIt->second->mpActor;

        objectIt = sObjectRegistry.erase(objectIt);
      }
      else
      {
        objectIt++;
      }
    }
  }
}