#include "axis.hpp"

Axis::Axis(glm::vec3 direction, float length) {
    vertices_ = {glm::vec3(0.0f), direction * length};
    edges_ = {0, 1};

    upload();
}
