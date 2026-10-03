#pragma once

#include "meshes.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>

enum class Phase { Idle, Animating, Finished };

class Scene {
public:
    virtual ~Scene() = default;

    virtual const char* title(Phase phase) const = 0;
    virtual glm::mat4   view() const            = 0;

    // progress: 0 - state before the transformation, 1 - after it.
    virtual void draw(const Shader& shader, float progress) const = 0;
};

class Axes {
public:
    explicit Axes(float length);
    void draw(const Shader& shader) const;

private:
    Axis x_;
    Axis y_;
    Axis z_;
};

// Items 1-2: the sphere is centered at the cone apex; both rotate about the coordinate Z axis.
class ConeSphereScene : public Scene {
public:
    ConeSphereScene();

    const char* title(Phase phase) const override;
    glm::mat4   view() const override;
    void        draw(const Shader& shader, float progress) const override;

private:
    Axes   axes_;
    Cone   cone_;
    Sphere sphere_;
};

// Items 3-4: cube and tetrahedron, tetrahedron vertex moved to the cube center, cube scaled by 1.5.
class CubeTetraScene : public Scene {
public:
    CubeTetraScene();

    const char* title(Phase phase) const override;
    glm::mat4   view() const override;
    void        draw(const Shader& shader, float progress) const override;

private:
    Axes        axes_;
    Cube        cube_;
    Tetrahedron tetra_;
};
