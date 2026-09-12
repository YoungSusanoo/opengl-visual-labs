#pragma once

#include <glm/ext/vector_float3.hpp>

#include <vector>

class Mesh {
public:
    ~Mesh();
    void draw() const;

protected:
    void upload();

    std::vector<glm::vec3> vertices_;
    std::vector<uint32_t>  edges_;

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
};
