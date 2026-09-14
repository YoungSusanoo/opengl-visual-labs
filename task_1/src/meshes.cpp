#include "meshes.hpp"

#include <GL/glew.h>

#include <cmath>
#include <numbers>
#include <ranges>

void Mesh::upload() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices_.size() * sizeof(glm::vec3)),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(edges_.size() * sizeof(uint32_t)),
                 edges_.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_LINES, static_cast<GLsizei>(edges_.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Axis::Axis(glm::vec3 direction, float length) {
    vertices_ = {glm::vec3(0.0f), direction * length};
    edges_    = {0, 1};

    upload();
}

Cone::Cone(float radius, float height, std::size_t segments) {
    vertices_.reserve(segments);

    for (std::size_t i : std::views::iota(std::size_t{0}, segments)) {
        const float theta = 2.0f * std::numbers::pi * static_cast<float>(i) / static_cast<float>(segments);
        vertices_.push_back({radius * std::cos(theta), 0.0f, radius * std::sin(theta)});
    }
    const auto apexIndex = static_cast<unsigned int>(segments);
    vertices_.push_back({0.0f, height, 0.0f});

    edges_.reserve(segments * 4);
    for (std::size_t i : std::views::iota(std::size_t{0}, segments)) {
        const auto a = static_cast<uint32_t>(i);
        const auto b = static_cast<uint32_t>((i + 1) % segments);
        edges_.insert(edges_.end(), {a, b, a, apexIndex});
    }

    upload();
}

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

Sphere::Sphere(float radius, std::size_t stacks, std::size_t slices) {
    const auto index = [slices](std::size_t stack, std::size_t slice) {
        return static_cast<uint32_t>(stack * slices + slice);
    };

    for (std::size_t i : std::views::iota(std::size_t{0}, stacks + 1)) {
        const float phi = std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(stacks);
        for (std::size_t j : std::views::iota(std::size_t{0}, slices)) {
            const float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / static_cast<float>(slices);
            vertices_.push_back({
                radius * std::sin(phi) * std::cos(theta),
                radius * std::cos(phi),
                radius * std::sin(phi) * std::sin(theta),
            });
        }
    }

    // Longitude arcs, pole to pole.
    for (std::size_t i : std::views::iota(std::size_t{0}, stacks)) {
        for (std::size_t j : std::views::iota(std::size_t{0}, slices)) {
            edges_.insert(edges_.end(), {index(i, j), index(i + 1, j)});
        }
    }
    // Latitude rings, skipping the poles (degenerate rings).
    for (std::size_t i : std::views::iota(std::size_t{1}, stacks)) {
        for (std::size_t j : std::views::iota(std::size_t{0}, slices)) {
            edges_.insert(edges_.end(), {index(i, j), index(i, (j + 1) % slices)});
        }
    }

    upload();
}

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
