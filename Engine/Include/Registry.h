#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global instance registry.
*/

template<typename V> std::map<std::string, V> sRegistry{};

template<typename V, typename ... Args> V& RegistryGetOrCreate(std::string const& key, Args&& ... args)
{
  auto const registryIt{ sRegistry<V>.find(key) };

  if (registryIt == sRegistry<V>.end())
  {
    auto const [insertIt, _]{ sRegistry<V>.emplace(key, V{ std::forward<Args>(args) ... }) };
    return insertIt->second;
  }

  return registryIt->second;
}