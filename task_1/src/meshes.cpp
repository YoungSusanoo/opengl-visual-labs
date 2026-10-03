#include "meshes.hpp"

#include <GL/glew.h>
#include <glm/ext/vector_float2.hpp>

#include <cmath>
#include <numbers>
#include <ranges>

namespace {

// First `count` points of a unit circle divided into `segments` equal steps.
// sin/cos of the step are computed once; every next point is the previous one
// rotated by the step: x' = c*x - s*y, y' = s*x + c*y.
std::vector<glm::vec2> circlePoints(std::size_t segments, std::size_t count) {
    const float step = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);
    const float c    = std::cos(step);
    const float s    = std::sin(step);

    std::vector<glm::vec2> points;
    points.reserve(count);
    glm::vec2 p(1.0f, 0.0f);
    while (points.size() < count) {
        points.push_back(p);
        p = {c * p.x - s * p.y, s * p.x + c * p.y};
    }
    return points;
}

}  // namespace

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
    vertices_.reserve(segments + 1);
    for (const glm::vec2& p : circlePoints(segments, segments)) {
        vertices_.push_back({radius * p.x, 0.0f, radius * p.y});
    }
    const auto apexIndex = static_cast<uint32_t>(segments);
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
    const float h = side / 2.0f;

    // Vertex i has bit 0 -> x, bit 1 -> y, bit 2 -> z set to the far side.
    vertices_.reserve(8);
    for (uint32_t i : std::views::iota(uint32_t{0}, uint32_t{8})) {
        vertices_.push_back({(i & 1u) ? h : -h, (i & 2u) ? side : 0.0f, (i & 4u) ? h : -h});
    }

    // Edges connect vertices that differ in exactly one bit.
    edges_.reserve(24);
    for (uint32_t i : std::views::iota(uint32_t{0}, uint32_t{8})) {
        for (uint32_t bit : {1u, 2u, 4u}) {
            if ((i & bit) == 0) {
                edges_.insert(edges_.end(), {i, i | bit});
            }
        }
    }

    upload();
}

Sphere::Sphere(float radius, std::size_t stacks, std::size_t slices) {
    const auto parallel = circlePoints(slices, slices);      // (cos theta, sin theta), theta in [0, 2pi)
    const auto meridian = circlePoints(2 * stacks, stacks);  // (cos phi, sin phi), phi in [0, pi)

    // Vertex 0 is the north pole, then rings 1..stacks-1, then the south pole.
    const uint32_t northPole = 0;
    const auto     southPole = static_cast<uint32_t>(1 + (stacks - 1) * slices);
    const auto     index     = [slices](std::size_t ring, std::size_t slice) {
        return static_cast<uint32_t>(1 + (ring - 1) * slices + slice);
    };

    vertices_.reserve(southPole + 1);
    vertices_.push_back({0.0f, radius, 0.0f});
    for (std::size_t i : std::views::iota(std::size_t{1}, stacks)) {
        const float y          = radius * meridian[i].x;
        const float ringRadius = radius * meridian[i].y;
        for (const glm::vec2& p : parallel) {
            vertices_.push_back({ringRadius * p.x, y, ringRadius * p.y});
        }
    }
    vertices_.push_back({0.0f, -radius, 0.0f});

    edges_.reserve(2 * slices * (2 * stacks - 1));
    for (std::size_t j : std::views::iota(std::size_t{0}, slices)) {
        const std::size_t next = (j + 1) % slices;
        edges_.insert(edges_.end(), {northPole, index(1, j)});
        for (std::size_t i : std::views::iota(std::size_t{1}, stacks)) {
            edges_.insert(edges_.end(), {index(i, j), index(i, next)});
            edges_.insert(edges_.end(), {index(i, j), i + 1 < stacks ? index(i + 1, j) : southPole});
        }
    }

    upload();
}

Tetrahedron::Tetrahedron(float edge) {
    // Regular tetrahedron standing on the XZ plane: base centroid at the origin, apex on the Y axis.
    // Base circumradius R = a/sqrt(3), height H = a*sqrt(2/3); only constants, no trigonometry.
    const float r      = edge * std::numbers::inv_sqrt3_v<float>;
    const float height = edge * std::numbers::sqrt2_v<float> * std::numbers::inv_sqrt3_v<float>;
    vertices_          = {
        {0.0f, height, 0.0f},  // apex
        {r, 0.0f, 0.0f},
        {-r / 2.0f, 0.0f, edge / 2.0f},
        {-r / 2.0f, 0.0f, -edge / 2.0f},
    };
    edges_ = {0, 1, 0, 2, 0, 3, 1, 2, 2, 3, 3, 1};

    upload();
}
