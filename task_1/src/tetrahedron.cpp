#include "tetrahedron.hpp"

#include <cmath>

Tetrahedron::Tetrahedron(float edgeLength) {
    const float a = edgeLength;
    // Vertex 0 sits at the local origin, so translating the whole mesh
    // places that vertex exactly at the translation target.
    vertices_ = {
        {0.0f, 0.0f, 0.0f},
        {a, 0.0f, 0.0f},
        {a * 0.5f, a * std::sqrt(3.0f) / 2.0f, 0.0f},
        {a * 0.5f, a * std::sqrt(3.0f) / 6.0f, a * std::sqrt(6.0f) / 3.0f},
    };
    edges_ = {0, 1, 0, 2, 0, 3, 1, 2, 1, 3, 2, 3};

    upload();
}
