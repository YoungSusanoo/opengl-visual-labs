#include "scenes.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <cstddef>

namespace {

constexpr float axisLength = 10.0f;

constexpr float       coneRadius          = 1.0f;
constexpr float       coneHeight          = 2.0f;
constexpr std::size_t coneSegments        = 32;
constexpr float       coneRotationDegrees = -60.0f;

constexpr float       sphereRadius = 0.5f;
constexpr std::size_t sphereStacks = 16;
constexpr std::size_t sphereSlices = 24;

constexpr float     cubeSide         = 2.0f;
constexpr float     cubeScaleFactor  = 1.5f;
constexpr float     tetraEdge        = 1.5f;
constexpr glm::vec3 tetraStartPos    = glm::vec3(2.5f, 0.0f, 0.0f);

void drawMesh(const Shader& shader, const Mesh& mesh, const glm::mat4& model, const glm::vec3& color) {
    shader.setMat4("model", model);
    shader.setVec3("color", color);
    mesh.draw();
}

float ease(float t) {
    return t * t * (3.0f - 2.0f * t);
}

// Eased local progress of a stage that occupies [begin, end] of the whole animation.
float stage(float progress, float begin, float end) {
    return ease(glm::clamp((progress - begin) / (end - begin), 0.0f, 1.0f));
}

}  // namespace

Axes::Axes(float length)
    : x_(glm::vec3(1.0f, 0.0f, 0.0f), length)
    , y_(glm::vec3(0.0f, 1.0f, 0.0f), length)
    , z_(glm::vec3(0.0f, 0.0f, 1.0f), length) { }

void Axes::draw(const Shader& shader) const {
    drawMesh(shader, x_, glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // red
    drawMesh(shader, y_, glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // green
    drawMesh(shader, z_, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // blue
}

ConeSphereScene::ConeSphereScene()
    : axes_(axisLength)
    , cone_(coneRadius, coneHeight, coneSegments)
    , sphere_(sphereRadius, sphereStacks, sphereSlices) { }

const char* ConeSphereScene::title(Phase phase) const {
    switch (phase) {
        case Phase::Idle:
            return "Сцена 1/2 — п.1: конус и сфера с центром в вершине конуса  [Пробел: п.2]";
        case Phase::Animating:
            return "Сцена 1/2 — поворот конуса и сферы на -60° вокруг оси Z";
        case Phase::Finished:
            return "Сцена 1/2 — конус и сфера повёрнуты на -60° вокруг оси Z  [Пробел: следующая сцена]";
    }
    return "";
}

glm::mat4 ConeSphereScene::view() const {
    const glm::vec3 target(-0.5f, 1.3f, 0.0f);
    return glm::lookAt(target + glm::vec3(1.5f, 1.7f, 8.0f), target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void ConeSphereScene::draw(const Shader& shader, float progress) const {
    const glm::vec3 apex(0.0f, coneHeight, 0.0f);
    const float     angle = glm::radians(coneRotationDegrees) * stage(progress, 0.0f, 1.0f);

    // Apply the same rotation around the coordinate Z axis to both objects.
    // The sphere center and the cone apex therefore remain coincident.
    const glm::mat4 rotation    = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 coneModel   = rotation;
    const glm::mat4 sphereModel = rotation * glm::translate(glm::mat4(1.0f), apex);

    axes_.draw(shader);
    drawMesh(shader, cone_, coneModel, glm::vec3(1.0f, 0.55f, 0.0f));      // orange
    drawMesh(shader, sphere_, sphereModel, glm::vec3(0.0f, 0.85f, 0.9f));  // cyan
}

CubeTetraScene::CubeTetraScene() : axes_(axisLength), cube_(cubeSide), tetra_(tetraEdge) { }

const char* CubeTetraScene::title(Phase phase) const {
    switch (phase) {
        case Phase::Idle:
            return "Сцена 2/2 — п.3: куб и тетраэдр  [Пробел: п.4]";
        case Phase::Animating:
            return "Сцена 2/2 — п.4: перенос тетраэдра в центр куба, масштабирование куба ×1.5";
        case Phase::Finished:
            return "Сцена 2/2 — п.4: вершина тетраэдра в центре куба, куб ×1.5  [Пробел: к первой сцене]";
    }
    return "";
}

glm::mat4 CubeTetraScene::view() const {
    // Looking straight at the front face through the cube center: the apex hits the middle of the screen
    // and scaling about the center grows the cube symmetrically.
    const glm::vec3 cubeCenter(0.0f, cubeSide / 2.0f, 0.0f);
    return glm::lookAt(cubeCenter + glm::vec3(0.0f, 0.0f, 7.5f), cubeCenter, glm::vec3(0.0f, 1.0f, 0.0f));
}

void CubeTetraScene::draw(const Shader& shader, float progress) const {
    const glm::vec3 cubeCenter(0.0f, cubeSide / 2.0f, 0.0f);

    // First half: the tetrahedron stands on the XZ plane next to the cube and moves until its apex lands
    // at the cube center. Its local origin is the base centroid, so the target position is center - apex.
    const glm::vec3 tetraTarget = cubeCenter - tetra_.apex();
    const glm::vec3 tetraPos    = glm::mix(tetraStartPos, tetraTarget, stage(progress, 0.0f, 0.5f));
    const glm::mat4 tetraModel  = glm::translate(glm::mat4(1.0f), tetraPos);

    // Second half: scale the cube about its center, so the center (and the tetrahedron vertex) stay in place.
    const float     cubeScale = glm::mix(1.0f, cubeScaleFactor, stage(progress, 0.5f, 1.0f));
    const glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), cubeCenter) *
                                glm::scale(glm::mat4(1.0f), glm::vec3(cubeScale)) *
                                glm::translate(glm::mat4(1.0f), -cubeCenter);

    axes_.draw(shader);
    drawMesh(shader, cube_, cubeModel, glm::vec3(0.2f, 0.9f, 0.2f));     // green
    drawMesh(shader, tetra_, tetraModel, glm::vec3(0.9f, 0.2f, 0.8f));  // magenta
}
