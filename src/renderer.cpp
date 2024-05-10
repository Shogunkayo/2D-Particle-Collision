#include "renderer.h"
#include <GL/gl.h>
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "ERROR::OPENGL::" << error << "::"
            << function << "::" << file << "::AT LINE " << line << "\n" ;
        return false;
    }

    return true;
}
