#include "circle.h"
#include "renderer.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


Circle::Circle(int num_circles, const float scr_width, const float scr_height)
    : num_circles(num_circles), shader("res/shaders/circle.shader") {
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

    for (int i = 0; i < num_circles; i++) {
        circleData[i].center_radius = glm::vec4(120.0f + i * 200.0f, 70.0f + i * 50.0f, 10.0f + i * 10.0f, 10.0f + i * 10.0f);
        circleData[i].velocity = glm::vec2(2.0f, 1.0f);
        circleData[i].direction = glm::vec2(1.0f, 1.0f);
        circleData[i].acceleration = glm::vec2(0.0f, 0.0f);
    }

    glm::mat4 proj = glm::ortho(0.0f, scr_width, 0.0f, scr_height, -1.0f, 1.0f);

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));
    Bind();
    {
        shader.SetUniformMat4f("u_MVP", proj);
        shader.SetUniform4fv("u_Color", glm::vec4(0.18f, 0.6f, 0.96f, 1.0f));

        for (unsigned int i = 0; i < num_circles; i++) {
            shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", circleData[i].center_radius);
        }

        GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0));

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

    for (unsigned int i = 0; i < num_circles; i++) {
        circleData[i].center_radius.x += float(circleData[i].velocity.x * circleData[i].direction.x);
        circleData[i].center_radius.y += float(circleData[i].velocity.y * circleData[i].direction.y);

        shader.SetUniform4fv("u_Updates[" + std::to_string(i) + "]", circleData[i].center_radius);
    }

    GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, num_circles));
}
