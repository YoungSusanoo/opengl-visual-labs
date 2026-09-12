#pragma once

#include "mesh.hpp"

class Axis : public Mesh {
public:
    Axis(glm::vec3 direction, float length);
};
