#include "scenes.hpp"
#include "shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t sceneCount       = 2;
constexpr double      animationSeconds = 2.0;

struct Presentation {
    std::size_t scene      = 0;
    Phase       phase      = Phase::Idle;
    double      start      = 0.0;
    bool        titleDirty = true;
};

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

// Space: start the animation of the current scene, or switch to the next scene once it has finished.
void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key != GLFW_KEY_SPACE || action != GLFW_PRESS) {
        return;
    }
    auto* presentation = static_cast<Presentation*>(glfwGetWindowUserPointer(window));
    switch (presentation->phase) {
        case Phase::Idle:
            presentation->phase = Phase::Animating;
            presentation->start = glfwGetTime();
            break;
        case Phase::Animating:
            return;
        case Phase::Finished:
            presentation->scene = (presentation->scene + 1) % sceneCount;
            presentation->phase = Phase::Idle;
            break;
    }
    presentation->titleDirty = true;
}

void run(GLFWwindow* window) {
    constexpr std::string_view shaderVert = "shaders/basic.vert";
    constexpr std::string_view shaderFrag = "shaders/basic.frag";

    Shader shader(std::string{shaderVert}, std::string{shaderFrag});

    const ConeSphereScene                         coneSphere;
    const CubeTetraScene                          cubeTetra;
    const std::array<const Scene*, sceneCount>    scenes{&coneSphere, &cubeTetra};

    Presentation presentation;
    glfwSetWindowUserPointer(window, &presentation);
    glfwSetKeyCallback(window, keyCallback);

    while (!glfwWindowShouldClose(window)) {
        float progress = 0.0f;
        if (presentation.phase == Phase::Animating) {
            progress = static_cast<float>((glfwGetTime() - presentation.start) / animationSeconds);
            if (progress >= 1.0f) {
                presentation.phase      = Phase::Finished;
                presentation.titleDirty = true;
            }
        }
        if (presentation.phase == Phase::Finished) {
            progress = 1.0f;
        }

        const Scene& scene = *scenes[presentation.scene];
        if (presentation.titleDirty) {
            glfwSetWindowTitle(window, scene.title(presentation.phase));
            presentation.titleDirty = false;
        }

        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width  = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        const float     aspect     = height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("view", scene.view());
        shader.setMat4("projection", projection);
        scene.draw(shader, progress);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

}  // namespace

int main() {
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
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // GL objects live inside run() so they are released while the context still exists.
    run(window);

    glfwTerminate();
    return 0;
}
