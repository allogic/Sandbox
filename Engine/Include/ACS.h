#pragma once

#include <Core.h>
#include <Types.h>
#include <Transform.h>

/*
* Type deduction utility.
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
  Actor*      mpActor      {};
  u64         mMask        {};
  Component** mppComponents{};
};
struct Actor
{
  Actor*    mpParent     {};
  Actor*    mppChilds[64]{};

  Transform mTransform   {};

  Object*   mpObject;

  Actor(Object* pObject) : mpObject{ pObject } {}
  
  virtual void OnEnable() {};
  virtual void OnDisable() {};
  virtual void OnUpdate(r32 timeDelta) {};
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnGizmo() {}
  virtual void OnImGui(r32 timeDelta) {}
};

static std::map<std::string, Object*> sObjectRegistry  {};
static u64                            sUniqueTypeCount {};
static std::map<u64, u64>             sIdentityRegistry{};

namespace ACS
{
  /*
  * Component helper utlities.
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
  
  constexpr static u64 ContainsComponentBits(s64 entityMask, s64 componentMask)
  {
    return (~(entityMask ^ componentMask) & componentMask) == componentMask;
  }

  /*
  * Actor management.
  */
  
  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A& Create(std::string const& actorName, Args&& ... args)
  {
    auto const objectIt{ sObjectRegistry.find(actorName) };
  
    if (objectIt == sObjectRegistry.end())
    {
      Object* pObject{ new Object };
  
      pObject->mMask = 0;
      pObject->mppComponents = new Component*[64];
  
      auto const [insertIt, _] { sObjectRegistry.emplace(actorName, pObject) };
  
      // create actor at least in order to maintain sub-component order initialization 
      pObject->mpActor = new A{ pObject, std::forward<Args>(args) ... };
  
      return *((A*)insertIt->second->mpActor);
    }
  
    return *((A*)objectIt->second->mpActor);
  }
  
  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A& CreateChild(Actor* pActor, u64 subActorIndex, std::string const& actorName, Args&& ... args)
  {
    pActor->mppChilds[subActorIndex] = &Create<A>(actorName, std::forward<Args>(args) ...);
    pActor->mppChilds[subActorIndex]->mpParent = pActor;

    return *((A*)pActor->mppChilds[subActorIndex]);
  }

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C& Attach(Actor* pActor, Args&& ... args)
  {
    s64 const componentIndex{ ComponentToIndex<C>() };
    s64 const componentMask{ (s64)1 << componentIndex };
  
    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {  
      return *((C*)pActor->mpObject->mppComponents[componentIndex]);
    }
    else
    {  
      pActor->mpObject->mMask |= componentMask;
      pActor->mpObject->mppComponents[componentIndex] = new C{ std::forward<Args>(args) ... };
  
      return *((C*)pActor->mpObject->mppComponents[componentIndex]);
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void Dispatch(std::function<void(Actor*)>&& predicate)
  {
    for (auto const [name, pObject] : sObjectRegistry)
    {
      predicate(pObject->mpActor);
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchFor(std::function<void(Actor*, typename Identity<Comps>::Ptr ...)>&& predicate)
  {
    s64 const componentMask{ (((s64)1 << ComponentToIndex<typename Identity<Comps>::Type>()) | ... | (s64)0) };

    for (auto const [name, pObject] : sObjectRegistry)
    {
      if (ContainsComponentBits(pObject->mMask, componentMask))
      {
        predicate
        (
          pObject->mpActor,
          (typename Identity<Comps>::Ptr)(pObject->mppComponents[ComponentToIndex<typename Identity<Comps>::Type>()])
          ...
        );
      }
    }
  }
}