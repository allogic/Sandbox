#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global instance registry.
*/

template<typename V> std::map<std::string, V> sInstanceRegistry{};

template<typename V, typename ... Args> V& RegistryGetOrCreate(std::string const& key, Args&& ... args)
{
  auto const instanceIt{ sInstanceRegistry<V>.find(key) };

  if (instanceIt == sInstanceRegistry<V>.end())
  {
    auto const [insertIt, _]{ sInstanceRegistry<V>.emplace(key, V{ std::forward<Args>(args) ... }) };
    return insertIt->second;
  }

  return instanceIt->second;
}