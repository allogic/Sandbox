#pragma once

#include <Vertex.h>

struct Mesh
{
  Mesh() {};
  Mesh(u32 numVertices, u32 numIndices)
    : mpVertices{ (Vertex*)std::malloc(sizeof(Vertex) * numVertices) }
    , mpIndices{ (u32*)std::malloc(sizeof(u32) * numIndices) } {}

  Vertex* mpVertices{};
  u32*    mpIndices {};
};