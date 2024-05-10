#include "circle.h"
#include "container.h"
#include "renderer.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <string>

Circle::Circle(int num_circles, const float scr_width, const float scr_height, struct ContainerBoundary containerBoundary)
    : num_circles(num_circles), shader("res/shaders/circle.shader"), containerBoundary(containerBoundary) {
    float vertices[] = {
        50.0f, 50.0f,
        50.0f, -50.0f,
        -50.0f, -50.0f,
        -50.0f, 50.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    float radius_list[25] = {10.0f, 30.0f, 10.0f, 20.0f, 10.0f, 30.0f, 10.0f, 20.0f, 10.0f, 20.0f, 10.0f, 20.0f,
        10.0f, 10.0f, 40.0f, 30.0f, 30.0f, 20.0f, 20.0f, 10.0f, 20.0f, 20.0f, 30.0f, 20.0f, 40.0f};
    float direction_list[6] = {1.0, -1.0, 1.0, 1.0, -1.0, 1.0};
    glm::vec4 color_list[4] = {
        glm::vec4(0.51f, 0.91f, 0.58f, 1.0f),
        glm::vec4(0.18f, 0.6f, 0.96f, 1.0f),
        glm::vec4(0.929f, 0.51f, 0.388f, 1.0f),
        glm::vec4(1.0f, 0.922f, 0.612f, 1.0f)
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr_velocity(75.0f, 100.0f);
    std::uniform_real_distribution<> distr_position_x(containerBoundary.left + 50.0f, containerBoundary.right - 50.0f);
    std::uniform_real_distribution<> distr_position_y(containerBoundary.bottom + 50.0f, containerBoundary.top - 50.0f);

    for (int i = 0; i < num_circles; i++) {
        circleData[i].center_radius = glm::vec4(float(distr_position_x(gen)), float(distr_position_y(gen)), radius_list[i % 25], radius_list[i % 25]);
        circleData[i].velocity = glm::vec2(float(distr_velocity(gen)), float(distr_velocity(gen)));
        circleData[i].direction = glm::vec2(direction_list[i % 6], direction_list[6 - (i % 6)]);
        circleData[i].acceleration = glm::vec2(0.0f, 0.0f);
    }

    glm::mat4 proj = glm::ortho(0.0f, scr_width, 0.0f, scr_height, -1.0f, 1.0f);

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));
    Bind();
    {
        shader.SetUniformMat4f("u_MVP", proj);

        for (unsigned int i = 0; i < num_circles; i++) {
            shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", circleData[i].center_radius);
            shader.SetUniform4fv("u_Colors[" + std::to_string(i) + "]", color_list[i % 4]);
        }

        GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*) 0));

        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    Unbind();
}

Circle::~Circle() {
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteBuffers(1, &EBO));
    GLCall(glDeleteVertexArrays(1, &VAO));
}

void Circle::Bind() {
    shader.Bind();
    GLCall(glBindVertexArray(VAO));
}

void Circle::Unbind() {
    shader.Unbind();
    GLCall(glBindVertexArray(0));
}

void Circle::Draw(float deltaTime) {
    Bind();

    for (unsigned int i = 0; i < num_circles; i++) {
        if (circleData[i].center_radius.x - circleData[i].center_radius.z <= containerBoundary.left || circleData[i].center_radius.x + circleData[i].center_radius.z >= containerBoundary.right) {
            circleData[i].direction.x *= -1.0;
        }

        if (circleData[i].center_radius.y - circleData[i].center_radius.z <= containerBoundary.bottom || circleData[i].center_radius.y + circleData[i].center_radius.z >= containerBoundary.top) {
            circleData[i].direction.y *= -1.0;
        }

        circleData[i].center_radius.x += float(circleData[i].velocity.x * circleData[i].direction.x * deltaTime);
        circleData[i].center_radius.y += float(circleData[i].velocity.y * circleData[i].direction.y * deltaTime);

        shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", circleData[i].center_radius);
    }

    GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_circles));
}
