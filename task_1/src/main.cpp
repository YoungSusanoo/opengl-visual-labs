#include "shader.hpp"
#include "axis.hpp"
#include "camera.hpp"
#include "cone.hpp"
#include "cube.hpp"
#include "sphere.hpp"
#include "tetrahedron.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

namespace {

struct InputState {
    Camera* camera   = nullptr;
    bool    dragging = false;
    double  lastX    = 0.0;
    double  lastY    = 0.0;
};

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto* input = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        input->dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &input->lastX, &input->lastY);
    }
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    auto* input = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (input->dragging) {
        input->camera->move(static_cast<float>(x - input->lastX), static_cast<float>(y - input->lastY));
    }
    input->lastX = x;
    input->lastY = y;
}

void scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    auto* input = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    input->camera->scroll(static_cast<float>(yoffset));
}
}  // namespace

int main() {
    constexpr float       coneRadius   = 1.0f;
    constexpr float       coneHeight   = 2.0f;
    constexpr std::size_t coneSegments = 32;

    constexpr float       sphereRadius = 0.5f;
    constexpr std::size_t sphereStacks = 16;
    constexpr std::size_t sphereSlices = 24;

    constexpr float cubeSide        = 1.0f;
    constexpr float cubeScaleFactor = 1.5f;

    constexpr float tetraEdge = 1.5f;

    constexpr float axisLength = 10.0f;

    constexpr glm::vec3 coneBasePos(-3.0f, 0.0f, 0.0f);
    constexpr glm::vec3 cubePos(3.0f, 0.0f, 0.0f);
    constexpr glm::vec3 cubeCenter(3.0f, cubeSide / 2.0f, 0.0f);

    constexpr float coneRotationDegrees = -60.0f;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Lab 1 - Variant 57", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
    if (glGenVertexArrays == nullptr || glCreateShader == nullptr) {
        std::cerr << "Failed to load required OpenGL functions via GLEW\n";
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    Camera     camera(glm::vec3(0.0f, 1.0f, 0.0f), 14.0f);
    InputState input{.camera = &camera};
    glfwSetWindowUserPointer(window, &input);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    Shader shader(std::string(SHADER_DIR) + "/basic.vert", std::string(SHADER_DIR) + "/basic.frag");

    // ---- Scene geometry (local space) ----
    Cone        cone(coneRadius, coneHeight, coneSegments);
    Sphere      sphere(sphereRadius, sphereStacks, sphereSlices);
    Cube        cube(cubeSide);
    Tetrahedron tetra(tetraEdge);

    Axis axisX(glm::vec3(1.0f, 0.0f, 0.0f), axisLength);
    Axis axisY(glm::vec3(0.0f, 1.0f, 0.0f), axisLength);
    Axis axisZ(glm::vec3(0.0f, 0.0f, 1.0f), axisLength);

    const glm::vec3 coneApexInitial = coneBasePos + glm::vec3(0.0f, coneHeight, 0.0f);
    const glm::mat4 sphereModel     = glm::translate(glm::mat4(1.0f), coneApexInitial);

    const glm::mat4 coneModel =
        glm::translate(glm::mat4(1.0f), coneApexInitial) *
        glm::rotate(glm::mat4(1.0f), glm::radians(coneRotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -coneHeight, 0.0f));

    const glm::mat4 cubeModel  = glm::scale(glm::translate(glm::mat4(1.0f), cubePos), glm::vec3(cubeScaleFactor));
    const glm::mat4 tetraModel = glm::translate(glm::mat4(1.0f), cubeCenter);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width  = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        const float     aspect     = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 view       = camera.getView();

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        const auto drawMesh = [&](const Mesh& mesh, const glm::mat4& model, const glm::vec3& color) {
            shader.setMat4("model", model);
            shader.setVec3("color", color);
            mesh.draw();
        };

        drawMesh(axisX, glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // red
        drawMesh(axisY, glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // green
        drawMesh(axisZ, glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // blue

        drawMesh(cone, coneModel, glm::vec3(1.0f, 0.55f, 0.0f));      // orange
        drawMesh(sphere, sphereModel, glm::vec3(0.0f, 0.85f, 0.9f));  // cyan
        drawMesh(cube, cubeModel, glm::vec3(0.2f, 0.9f, 0.2f));       // green
        drawMesh(tetra, tetraModel, glm::vec3(0.9f, 0.2f, 0.8f));     // magenta

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
