#include "cone.hpp"

#include <cmath>
#include <numbers>
#include <ranges>

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
