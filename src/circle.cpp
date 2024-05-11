#include "circle.h"
#include "container.h"
#include "renderer.h"
#include <algorithm>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <unordered_set>
#include <string>
#include <vector>
#include <omp.h>

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

    float radius1 = 4.0f;
    float radius2 = 6.0f;
    float radius3 = 8.0f;
    float radius4 = 10.0f;
    float radius5 = 12.0f;
    float radius6 = 14.0f;

    float radius_list[25] = {radius1, radius5, radius1, radius2, radius1, radius4, radius1, radius2, radius1, radius2, radius1, radius2,
        radius1, radius1, radius6, radius4, radius3, radius2, radius2, radius1, radius2, radius2, radius4, radius2, radius3};
    glm::vec4 color_list[4] = {
        glm::vec4(0.51f, 0.91f, 0.58f, 1.0f),
        glm::vec4(0.18f, 0.6f, 0.96f, 1.0f),
        glm::vec4(0.929f, 0.51f, 0.388f, 1.0f),
        glm::vec4(1.0f, 0.922f, 0.612f, 1.0f)
    };

    // DEFAULT COLOR FOR SWEEP AND PRUNE
    // AS CIRCLE DATA GETS SORTED INPLACE
    // ----------------------------------
    glm::vec4 default_color(0.18f, 0.6f, 0.96f, 1.0f);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr_velocity(-80.0f, 80.0f);
    std::uniform_real_distribution<> distr_position_x(containerBoundary.left + 50.0f, containerBoundary.right - 50.0f);
    std::uniform_real_distribution<> distr_position_y(containerBoundary.bottom + 50.0f, containerBoundary.top - 50.0f);

    for (int i = 0; i < num_circles; i++) {
        circleData[i].center_radius = glm::vec4(float(distr_position_x(gen)), float(distr_position_y(gen)), radius_list[i % 25], radius_list[i % 25]);
        circleData[i].velocity = glm::vec2(float(distr_velocity(gen)), float(distr_velocity(gen)));
    }

    /*
    circleData[0].center_radius = glm::vec4(300.0f, 300.0f, 10.0, 20.0);
    circleData[1].center_radius = glm::vec4(500.0f, 500.0f, 30.0, 30.0);

    circleData[0].velocity = glm::vec2(60.0f, 0.0f);
    circleData[1].velocity = glm::vec2(0.0f, -50.0f);
    */

    glm::mat4 proj = glm::ortho(0.0f, scr_width, 0.0f, scr_height, -1.0f, 1.0f);

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));
    Bind();
    {
        shader.SetUniformMat4f("u_MVP", proj);

        for (unsigned int i = 0; i < num_circles; i++) {
            shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", circleData[i].center_radius);
            // shader.SetUniform4fv("u_Colors[" + std::to_string(i) + "]", color_list[i % 4]);
            shader.SetUniform4fv("u_Colors[" + std::to_string(i) + "]", default_color);
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
        float x2 = circleData[j].center_radius.x - circleData[i].center_radius.x;
        float y2 = circleData[j].center_radius.y - circleData[i].center_radius.y;
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
                    Update(deltaTime, i, j);
                }
            }
        }
    }
}

void Circle::SweepAndPrune(float deltaTime) {
    std::sort(circleData, circleData + num_circles, [](const CircleData& a, const CircleData& b) {
        return a.center_radius.x < b.center_radius.x;
    });

    std::unordered_set<unsigned int> colliding_circles;

    #pragma omp for
    for (unsigned int i = 0; i < num_circles - 1; i++) {
        struct CircleData a = circleData[i];
        struct CircleData b = circleData[i+1];
        if ((a.center_radius.x - a.center_radius.z <= b.center_radius.x + b.center_radius.z) &&
            (a.center_radius.x + a.center_radius.z >= b.center_radius.x - b.center_radius.z)) {
            colliding_circles.insert(i);
            colliding_circles.insert(i + 1);
        }
    }

    unsigned int n = colliding_circles.size();
    std::vector<unsigned int> colliding_circles_vec;
    colliding_circles_vec.reserve(colliding_circles.size());

    for (auto it = colliding_circles.begin(); it != colliding_circles.end();) {
        colliding_circles_vec.push_back(std::move(colliding_circles.extract(it++).value()));
    }

    for (unsigned int i = 0; i < n; i++) {
        #pragma omp for
        for (unsigned int j = 0; j < n; j++) {
            if (i != j) {
                float x2 = circleData[colliding_circles_vec[j]].center_radius.x - circleData[colliding_circles_vec[i]].center_radius.x;
                float y2 = circleData[colliding_circles_vec[j]].center_radius.y - circleData[colliding_circles_vec[i]].center_radius.y;
                float r2 = circleData[colliding_circles_vec[j]].center_radius.z + circleData[colliding_circles_vec[i]].center_radius.z;

                if ((x2 * x2) + (y2 * y2) <= (r2 * r2)) {
                    Update(deltaTime, colliding_circles_vec[i], colliding_circles_vec[j]);
                }
            }
        }
    }
}
