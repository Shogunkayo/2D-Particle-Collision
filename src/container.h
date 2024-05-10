#pragma once
#include <string>
#include "shader.h"
#include <glm/glm.hpp>

struct ContainerBoundary {
    float top;
    float bottom;
    float left;
    float right;
};

class Container {
private:
    Shader shader;
    unsigned int VBO;
    unsigned int VAO;

public:
    struct ContainerBoundary boundary;

    Container(const float width, const float height, const float scr_width, const float scr_height, const std::string& filepath, const glm::vec4& color);
    ~Container();

    void Bind();
    void Unbind();
    void Draw();
};
