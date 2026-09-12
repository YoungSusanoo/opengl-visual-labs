#include "sphere.hpp"

#include <cmath>
#include <numbers>
#include <ranges>

Sphere::Sphere(float radius, std::size_t stacks, std::size_t slices) {
    const auto index = [slices](std::size_t stack, std::size_t slice) {
        return static_cast<uint32_t>(stack * slices + slice);
    };

    for (std::size_t i : std::views::iota(std::size_t{0}, stacks + 1)) {
        const float phi = std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(stacks);
        for (std::size_t j : std::views::iota(std::size_t{0}, slices)) {
            const float theta =
                2.0f * std::numbers::pi_v<float> * static_cast<float>(j) / static_cast<float>(slices);
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
