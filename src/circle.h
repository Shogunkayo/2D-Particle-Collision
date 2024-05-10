#pragma once

#include "shader.h"

struct CircleData {
    glm::vec4 center_radius;
    glm::vec2 velocity;
    glm::vec2 direction;
    glm::vec2 acceleration;
};

class Circle {
private:
    Shader shader;
    int num_circles;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    struct CircleData circleData[4];

public:
    Circle(int num_circles, const float scr_width, const float scr_height);
    ~Circle();

    void Bind();
    void Unbind();
    void Draw();
};
