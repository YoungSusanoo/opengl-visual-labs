#pragma once

#include "mesh.hpp"

#include <cstddef>

class Sphere : public Mesh {
public:
    Sphere(float radius, std::size_t stacks, std::size_t slices);
};
