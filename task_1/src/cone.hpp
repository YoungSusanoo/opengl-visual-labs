#pragma once

#include "mesh.hpp"

#include <cstddef>

class Cone : public Mesh {
public:
    Cone(float radius, float height, std::size_t segments);
};
