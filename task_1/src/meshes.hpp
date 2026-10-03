#pragma once

#include <glm/ext/vector_float3.hpp>

#include <cstddef>
#include <cstdint>
#include <vector>

class Mesh {
public:
    Mesh()                       = default;
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;
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

class Axis : public Mesh {
public:
    Axis(glm::vec3 direction, float length);
};

class Cone : public Mesh {
public:
    Cone(float radius, float height, std::size_t segments);
};

class Cube : public Mesh {
public:
    explicit Cube(float side);
};

class Sphere : public Mesh {
public:
    Sphere(float radius, std::size_t stacks, std::size_t slices);
};

class Tetrahedron : public Mesh {
public:
    explicit Tetrahedron(float edge);

    glm::vec3 apex() const { return vertices_[0]; }
};
