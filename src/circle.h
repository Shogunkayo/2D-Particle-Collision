#pragma once

#include "container.h"
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

    struct CircleData circleData[50];
    struct ContainerBoundary containerBoundary;

public:
    Circle(int num_circles, const float scr_width, const float scr_height, struct ContainerBoundary containerBoundary);
    ~Circle();

    void Bind();
    void Unbind();
    void Draw();
    void Update(float deltaTime);
};
