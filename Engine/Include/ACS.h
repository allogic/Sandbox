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
  Actor(Object* pObject) : mpObject{ pObject } {}
  
  Object* mpObject{};
  
  virtual void OnEnable() {};
  virtual void OnDisable() {};
  virtual void OnUpdate(r32 timeDelta) {};
  virtual void OnUpdateFixed(r32 timeDelta) {}
  virtual void OnGizmos(r32 timeDelta) {}
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
  void DispatchIf(std::function<void(Actor*)>&& predicate)
  {
    s64 const componentMask{ (((s64)1 << ComponentToIndex<typename Identity<Comps>::Type>()) | ... | (s64)0) };
  
    for (auto const [name, pObject] : sObjectRegistry)
    {
      if (ContainsComponentBits(pObject->mMask, componentMask))
      {
        predicate(pObject->mpActor);
      }
    }
  }
  
  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchFor(std::function<void(typename Identity<Comps>::Ptr ...)>&& predicate)
  {
    s64 const componentMask{ (((s64)1 << ComponentToIndex<typename Identity<Comps>::Type>()) | ... | (s64)0) };

    for (auto const [name, pObject] : sObjectRegistry)
    {
      if (ContainsComponentBits(pObject->mMask, componentMask))
      {
        predicate
        (
          (typename Identity<Comps>::Ptr)(pObject->mppComponents[ComponentToIndex<typename Identity<Comps>::Type>()])
          ...
        );
      }
    }
  }
}