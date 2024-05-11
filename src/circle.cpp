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

    float radius1 = 10.0f;
    float radius2 = 15.0f;
    float radius3 = 20.0f;
    float radius4 = 25.0f;
    float radius5 = 30.0f;
    float radius6 = 35.0f;

    float radius_list[25] = {radius1, radius5, radius1, radius2, radius1, radius4, radius1, radius2, radius1, radius2, radius1, radius2,
        radius1, radius1, radius6, radius4, radius3, radius2, radius2, radius1, radius2, radius2, radius4, radius2, radius3};
    float direction_list[6] = {1.0, -1.0, 1.0, 1.0, -1.0, 1.0};
    glm::vec4 color_list[4] = {
        glm::vec4(0.51f, 0.91f, 0.58f, 1.0f),
        glm::vec4(0.18f, 0.6f, 0.96f, 1.0f),
        glm::vec4(0.929f, 0.51f, 0.388f, 1.0f),
        glm::vec4(1.0f, 0.922f, 0.612f, 1.0f)
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr_velocity(30.0f, 40.0f);
    std::uniform_real_distribution<> distr_position_x(containerBoundary.left + 50.0f, containerBoundary.right - 50.0f);
    std::uniform_real_distribution<> distr_position_y(containerBoundary.bottom + 50.0f, containerBoundary.top - 50.0f);

    for (int i = 0; i < num_circles; i++) {
        circleData[i].center_radius = glm::vec4(float(distr_position_x(gen)), float(distr_position_y(gen)), radius_list[i % 25], radius_list[i % 25]);
        circleData[i].velocity = glm::vec2(float(distr_velocity(gen)), float(distr_velocity(gen)));
        circleData[i].direction = glm::vec2(direction_list[i % 6], direction_list[6 - (i % 6)]);
        circleData[i].acceleration = glm::vec2(0.0f, 0.0f);
    }

    circleData[0].center_radius = glm::vec4(300.0f, 200.0f, 20.0, 20.0);
    circleData[1].center_radius = glm::vec4(600.0f, 200.0f, 30.0, 20.0);

    circleData[0].velocity = glm::vec2(30.0f, 0.0f);
    circleData[1].velocity = glm::vec2(50.0f, 0.0f);

    circleData[0].direction = glm::vec2(1.0f, 1.0f);
    circleData[1].direction = glm::vec2(-1.0f, -1.0f);


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

void Circle::Draw() {
    Bind();
    GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_circles));
}

void Circle::Update(float deltaTime) {
    Bind();
    for (unsigned int i = 0; i < num_circles; i++) {
        struct CircleData& cd = circleData[i];
        if (cd.center_radius.x - cd.center_radius.z <= containerBoundary.left || cd.center_radius.x + cd.center_radius.z >= containerBoundary.right) {
            cd.direction.x *= -1.0;
        }

        if (cd.center_radius.y - cd.center_radius.z <= containerBoundary.bottom || cd.center_radius.y + cd.center_radius.z >= containerBoundary.top) {
            cd.direction.y *= -1.0;
        }

        cd.center_radius.x += float(cd.velocity.x * cd.direction.x * deltaTime);
        cd.center_radius.y += float(cd.velocity.y * cd.direction.y * deltaTime);

        shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", cd.center_radius);
    }
}

void Circle::Update(float deltaTime, const unsigned int i, const unsigned int j) {
    std::cout << "COLLISION UPDATE\n";
    circleData[i].direction *= -1;
    circleData[i].center_radius.x += float(circleData[i].velocity.x * circleData[i].direction.x * deltaTime);
    circleData[i].center_radius.y += float(circleData[j].velocity.y * circleData[j].direction.y * deltaTime);

    circleData[j].direction *= -1;
    circleData[j].center_radius.x += float(circleData[j].velocity.x * circleData[j].direction.x * deltaTime);
    circleData[j].center_radius.y += float(circleData[j].velocity.y * circleData[j].direction.y * deltaTime);
}

void Circle::NaiveCollision(float deltaTime) {
    for (unsigned int i = 0; i < num_circles; i++) {
        for (unsigned int j = 0; j < num_circles; j++) {
            if (i != j) {
                float x2 = circleData[i].center_radius.x - circleData[j].center_radius.x;
                float y2 = circleData[i].center_radius.y - circleData[j].center_radius.y;
                float r2 = circleData[i].center_radius.z + circleData[j].center_radius.z;
                if ((x2 * x2) + (y2 * y2) <= (r2 * r2)) {
                    float m1 = circleData[i].center_radius.z;
                    float m2 = circleData[j].center_radius.z;
                    float v1x = circleData[i].velocity.x;
                    float v2x = circleData[j].velocity.x;
                    float v1y = circleData[i].velocity.y;
                    float v2y = circleData[j].velocity.y;

                    std::cout << "BEFORE::Circle 1::velocity: " << circleData[0].velocity.x << " " << circleData[0].velocity.y << "\n";
                    std::cout << "BEFORE::Circle 2::velocity: " << circleData[1].velocity.x << " " << circleData[1].velocity.y << "\n";
                    circleData[i].velocity.x = (((m1 - m2) * v1x) + (2 * m2 * v2x)) / r2;
                    circleData[i].velocity.y = (((m1 - m2) * v1y) + (2 * m2 * v2y)) / r2;

                    circleData[j].velocity.x = ((2 * m1 * v1x) + ((m2 - m1) * v2x)) / r2;
                    circleData[j].velocity.y = ((2 * m1 * v1y) + ((m2 - m1) * v2y)) / r2;

                    std::cout << "AFTER::Circle 1::velocity: " << circleData[0].velocity.x << " " << circleData[0].velocity.y << "\n";
                    std::cout << "AFTER::Circle 2::velocity: " << circleData[1].velocity.x << " " << circleData[1].velocity.y << "\n";

                    Update(deltaTime, i, j);
                }
            }
        }
    }
}
