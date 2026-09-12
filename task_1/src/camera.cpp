#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

Camera::Camera(glm::vec3 center, float radius) : center_{center}, radius_{radius} { }

glm::mat4 Camera::getView() const {
    glm::vec3 eye_view = center_ + radius_ * glm::vec3(std::sin(azimuth_) * std::cos(elevation_),
                                                       std::sin(elevation_),
                                                       std::cos(azimuth_) * std::cos(elevation_));
    return glm::lookAt(eye_view, center_, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::move(float dx, float dy) {
    constexpr float sensitivity = 0.1f;
    azimuth_ -= dx * sensitivity;
    elevation_ = std::clamp(elevation_ + sensitivity * dy, -1.5f, 1.5f);
}

void Camera::scroll(float radius) {
    radius_ = std::clamp(radius_ - radius, 2.0f, 60.0f);
}
