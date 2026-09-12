#include "cube.hpp"

Cube::Cube(float side) {
    const float h = side / 2;
    vertices_     = {
        {-h, 0.0f, -h},
        {h, 0.0f, -h},
        {h, side, -h},
        {-h, side, -h},
        {-h, 0.0f, h},
        {h, 0.0f, h},
        {h, side, h},
        {-h, side, h},
    };
    edges_ = {
        0, 1, 1, 2, 2, 3, 3, 0,  // back face
        4, 5, 5, 6, 6, 7, 7, 4,  // front face
        0, 4, 1, 5, 2, 6, 3, 7,  // connecting edges
    };

    upload();
}
