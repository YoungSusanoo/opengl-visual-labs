#include "tetrahedron.hpp"

#include <cmath>

Tetrahedron::Tetrahedron(float edge) {
    float height = std::sqrt(3.0f) * edge / 2.0f;
    vertices_    = {
        {0.0f, 0.0f, 0.0f},
        {2.0f * height / 3.0f, height, 0.0f},
        {-height / 3.0f, height, -2.0f * height / 3.0f},
        {-height / 3.0f, height, 2.0f * height / 3.0f},
    };
    edges_ = {0, 1, 0, 2, 0, 3, 1, 2, 1, 3, 2, 3};

    upload();
}
