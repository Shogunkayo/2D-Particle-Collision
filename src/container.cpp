#include "renderer.h"
#include "container.h"
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/gtc/matrix_transform.hpp>

Container::Container(const float width, const float height, const float scr_width, const float scr_height, const glm::vec4& color)
    : shader("res/shaders/container.shader") {
    float offset_x = (scr_width - width) / 2;
    float offset_y = (scr_height - height) / 2;

    boundary = { offset_y, offset_y + height, offset_x, offset_x + width };

    float vertices[] = {
        boundary.right, boundary.top,
        boundary.right, boundary.bottom,
        boundary.left, boundary.bottom,

        boundary.right, boundary.top,
        boundary.left, boundary.top,
        boundary.left, boundary.bottom
    };

    glm::mat4 proj = glm::ortho(0.0f, scr_width, 0.0f, scr_height, -1.0f, 1.0f);

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));

    Bind();
    {
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        {
            shader.SetUniformMat4f("u_MVP", proj);
            shader.SetUniform4fv("u_Color", color);

            GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
            GLCall(glEnableVertexAttribArray(0));
            GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0));
        }
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
    Unbind();
}

Container::~Container() {
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteVertexArrays(1, &VAO));
}

void Container::Bind() {
    shader.Bind();
    GLCall(glBindVertexArray(VAO));
}

void Container::Unbind() {
    shader.Unbind();
    GLCall(glBindVertexArray(0));
}

void Container::Draw() {
    Bind();
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}
