#pragma once

#include <Core.h>
#include <Types.h>

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
* Actor component system.
*/

struct Actor;

struct Component
{
  
};
struct Object
{
  Actor*      mpActor       {};
  u64         mComponentMask{};
  Component** mppComponents {};
};
struct Actor
{
  Actor(Object* pObject) : mpObject{ pObject } {}
  
  Object* mpObject{};
  
  virtual void OnEnable() {};
  virtual void OnDisable() {};
  virtual void OnUpdate(r32 timeDelta) {};
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnGizmos(r32 timeDelta) {}
};

static std::map<std::string, Object*> sObjectRegistry  {};
static u64                            sUniqueTypeCount {};
static std::map<u64, u64>             sIdentityRegistry{};

namespace ACS
{
  /*
  * Component helper utlities.
  */
  
  template<typename C, u32 AsMaskBit = 0>
  requires std::is_base_of_v<Component, C>
  u64 ComponentToIdentity()
  {
    auto const identityIt{ sIdentityRegistry.find(typeid(C).hash_code()) };
  
    if (identityIt == sIdentityRegistry.end())
    {
      auto const [insertIt, _]{ sIdentityRegistry.emplace(typeid(C).hash_code(), sUniqueTypeCount++) };
      return AsMaskBit ? (u64)1 << insertIt->second : insertIt->second;
    }
  
    return AsMaskBit ? (u64)1 << identityIt->second : identityIt->second;
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
  
      pObject->mComponentMask = 0;
      pObject->mppComponents = (Component**)std::malloc(sizeof(Component*) * 64);
      
      std::memset(pObject->mppComponents, 0, sizeof(Component*) * 64);
  
      auto const [insertIt, _] { sObjectRegistry.emplace(actorName, pObject) };
  
      // create actor at least in order to maintain sub-component order initialization 
      pObject->mpActor = new A{ pObject, std::forward<Args>(args) ... };
  
      return *((A*)insertIt->second->mpActor);
    }
  
    return *((A*)objectIt->second->mpActor);
  }
  
  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C& Attach(Actor* pActor, Args&& ... args)
  {
    u64 const index{ ComponentToIdentity<C, 0>() };
    u64 const mask{ ComponentToIdentity<C, 1>() };
  
    if (pActor->mpObject->mComponentMask & mask)
    {
      // component exists
  
      return *((C*)pActor->mpObject->mppComponents[index]);
    }
    else
    {
      // component does not exist
  
      pActor->mpObject->mComponentMask |= mask;
      pActor->mpObject->mppComponents[index] = new C{ std::forward<Args>(args) ... };
  
      return *((C*)pActor->mpObject->mppComponents[index]);
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchIf(std::function<void(Actor*)>&& predicate)
  {
    u64 const mask{ (ComponentToIdentity<typename Identity<Comps>::Type, 1>() | ... | (u64)0) };
  
    for (auto const [name, pObject] : sObjectRegistry)
    {
      if (pObject->mComponentMask & mask)
      {
        predicate(pObject->mpActor);
      }
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchFor(std::function<void(typename Identity<Comps>::Ptr ...)>&& predicate)
  {  
    u64 const mask{ (ComponentToIdentity<typename Identity<Comps>::Type, 1>() | ... | (u64)0) };
  
    for (auto const [name, pObject] : sObjectRegistry)
    {
      if (pObject->mComponentMask & mask)
      {
        predicate
        (
          (typename Identity<Comps>::Ptr)(pObject->mppComponents[ComponentToIdentity<typename Identity<Comps>::Type, 0>()])
          ...
        );
      }
    }
  }
}