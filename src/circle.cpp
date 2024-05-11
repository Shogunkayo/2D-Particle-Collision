#include "circle.h"
#include "container.h"
#include "renderer.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
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
    glm::vec4 color_list[4] = {
        glm::vec4(0.51f, 0.91f, 0.58f, 1.0f),
        glm::vec4(0.18f, 0.6f, 0.96f, 1.0f),
        glm::vec4(0.929f, 0.51f, 0.388f, 1.0f),
        glm::vec4(1.0f, 0.922f, 0.612f, 1.0f)
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr_velocity(-80.0f, 80.0f);
    std::uniform_real_distribution<> distr_position_x(containerBoundary.left + 50.0f, containerBoundary.right - 50.0f);
    std::uniform_real_distribution<> distr_position_y(containerBoundary.bottom + 50.0f, containerBoundary.top - 50.0f);

    for (int i = 0; i < num_circles; i++) {
        circleData[i].center_radius = glm::vec4(float(distr_position_x(gen)), float(distr_position_y(gen)), radius_list[i % 25], radius_list[i % 25]);
        circleData[i].velocity = glm::vec2(float(distr_velocity(gen)), float(distr_velocity(gen)));
        circleData[i].acceleration = glm::vec2(0.0f, 0.0f);
    }

    circleData[0].center_radius = glm::vec4(300.0f, 300.0f, 10.0, 20.0);
    circleData[1].center_radius = glm::vec4(500.0f, 500.0f, 30.0, 30.0);

    circleData[0].velocity = glm::vec2(60.0f, 0.0f);
    circleData[1].velocity = glm::vec2(0.0f, -50.0f);

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
            cd.velocity.x *= -1.0;
        }

        if (cd.center_radius.y - cd.center_radius.z <= containerBoundary.bottom || cd.center_radius.y + cd.center_radius.z >= containerBoundary.top) {
            cd.velocity.y *= -1.0;
        }

        cd.center_radius.x += float(cd.velocity.x * deltaTime);
        cd.center_radius.y += float(cd.velocity.y * deltaTime);

        shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", cd.center_radius);
    }
}

void Circle::Update(float deltaTime, const unsigned int i, const unsigned int j) {
        //std::cout << "Overlap\n";
        circleData[i].center_radius.x += float(circleData[i].velocity.x * deltaTime);
        circleData[i].center_radius.y += float(circleData[i].velocity.y * deltaTime);
        
        circleData[j].center_radius.x += float(circleData[j].velocity.x * deltaTime);
        circleData[j].center_radius.y += float(circleData[j].velocity.y * deltaTime);

}

void Circle::NaiveCollision(float deltaTime) {
    for (unsigned int i = 0; i < num_circles; i++) {
        for (unsigned int j = 0; j < num_circles; j++) {
            if (i != j) {
                float x2 = circleData[j].center_radius.x - circleData[i].center_radius.x;
                float y2 = circleData[j].center_radius.y - circleData[i].center_radius.y;
                float r2 = circleData[j].center_radius.z + circleData[i].center_radius.z;
                if ((x2 * x2) + (y2 * y2) <= (r2 * r2)) {

                    /*
                    std::cout << "BEFORE::Circle 1::velocity: " << circleData[i].velocity.x << " " << circleData[i].velocity.y << "\n";
                    std::cout << "BEFORE::Circle 1::center: " << circleData[i].center_radius.x << " " << circleData[i].center_radius.y << "\n";
                    std::cout << "BEFORE::Circle 2::velocity: " << circleData[j].velocity.x << " " << circleData[j].velocity.y << "\n";
                    std::cout << "BEFORE::Circle 2::center: " << circleData[j].center_radius.x << " " << circleData[j].center_radius.y << "\n";
                    */

                    float m1 = circleData[i].center_radius.z;
                    float m2 = circleData[j].center_radius.z;
                    float v1x = circleData[i].velocity.x;
                    float v2x = circleData[j].velocity.x;
                    float v1y = circleData[i].velocity.y;
                    float v2y = circleData[j].velocity.y;

                    glm::vec2 v1 = circleData[i].velocity;
                    glm::vec2 v2 = circleData[j].velocity;

                    glm::vec2 normal = glm::vec2(x2, y2);
                    glm::vec2 unit_normal = normal / glm::length(normal);
                    glm::vec2 unit_tangent = glm::vec2(-unit_normal.y, unit_normal.x);

                    float v1_normal = glm::dot(unit_normal, v1);
                    float v1_tangent = glm::dot(unit_tangent, v1);
                    float v2_normal = glm::dot(unit_normal, v2);
                    float v2_tangent = glm::dot(unit_tangent, v2);

                    float v1_f_normal = float(((v1_normal * (m1 - m2)) + (2 * m2 * v2_normal)) / (m1 + m2)); 
                    float v2_f_normal = float(((v2_normal * (m2 - m1)) + (2 * m1 * v1_normal)) / (m1 + m2));

                    glm::vec2 v1_f_normal_vec = v1_f_normal * unit_normal;
                    glm::vec2 v1_f_tangent_vec = v1_tangent * unit_tangent;

                    glm::vec2 v2_f_normal_vec = v2_f_normal * unit_normal;
                    glm::vec2 v2_f_tangent_vec = v2_tangent * unit_tangent;

                    circleData[i].velocity = v1_f_normal_vec + v1_f_tangent_vec;
                    circleData[j].velocity = v2_f_normal_vec + v2_f_tangent_vec;

                    /*
                    std::cout <<"\nINTERMEDIATES::\n";
                    std::cout << "Normal: "<< normal.x << " " << normal.y << "\n";
                    std::cout << "Unit_normal: " << unit_normal.x << " " << unit_normal.y << "\n";
                    std::cout << "Unit_tangent: " << unit_tangent.x << " " << unit_tangent.y << "\n";
                    std::cout << "v1n: " << v1_normal << "\n";
                    std::cout << "v1t: " << v1_tangent << "\n";
                    std::cout << "v2n: " << v2_normal << "\n";
                    std::cout << "v2t: " << v2_tangent << "\n";

                    std::cout << "v1fn: " << v1_f_normal << "\n";
                    std::cout << "v2fn: " << v2_f_normal << "\n";

                    std::cout << "v1fnv: " << v1_f_normal_vec.x << " " << v1_f_normal_vec.y << "\n";
                    std::cout << "v2fnv: " << v2_f_normal_vec.x << " " << v2_f_normal_vec.y << "\n";
                    std::cout << "v1ftv: " << v1_f_tangent_vec.x << " " << v1_f_tangent_vec.y << "\n";
                    std::cout << "v2ftv: " << v2_f_tangent_vec.x << " " << v2_f_tangent_vec.y << "\n";
                    */

                    /*
                    circleData[i].velocity.x = float((((m1 - m2) * v1x) + (2 * m2 * v2x)) / r2);
                    circleData[i].velocity.y = float((((m1 - m2) * v1y) + (2 * m2 * v2y)) / r2);

                    circleData[j].velocity.x = float(((2 * m1 * v1x) + ((m2 - m1) * v2x)) / r2);
                    circleData[j].velocity.y = float(((2 * m1 * v1y) + ((m2 - m1) * v2y)) / r2);
                    */

                    /*
                    std::cout << "AFTER::Circle 1::velocity: " << circleData[i].velocity.x << " " << circleData[i].velocity.y << "\n";
                    std::cout << "AFTER::Circle 2::velocity: " << circleData[j].velocity.x << " " << circleData[j].velocity.y << "\n";
                    */

                    Update(deltaTime, i, j);
                }
            }
        }
    }
}
