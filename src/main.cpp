#include <cassert>
#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "container.h"
#include "renderer.h"
#include "circle.h"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main (int argc, char *argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hehehehaw", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        Container container(1680.0f, 945.0f, SCR_WIDTH, SCR_HEIGHT, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        Container containerBG(1685.0f, 950.0f, SCR_WIDTH, SCR_HEIGHT, glm::vec4(0.635f, 0.714f, 0.455f, 1.0f));
        Circle circle(500, SCR_WIDTH, SCR_HEIGHT, container.boundary);

        // deltaTime
        // -------------------
        float deltaTime = 1.0f / 144.0f;
        // float lastFrame = 0.0f;

        while (!glfwWindowShouldClose(window)) {
            // input
            // -----
            processInput(window);

            // calculate delta time
            // --------------------
            // float currentFrame = glfwGetTime();
            // deltaTime = currentFrame - lastFrame;
            // lastFrame = currentFrame;

            // render
            // ------
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            containerBG.Draw();
            container.Draw();
            circle.NaiveCollision(deltaTime);
            // circle.SweepAndPrune(deltaTime);
            circle.Update(deltaTime);
            circle.Draw();

            // swap buffers and poll IO events
            // -------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
