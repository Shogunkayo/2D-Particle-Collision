#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
    private:
        std::string m_FilePath;
        unsigned int m_RendererID;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        unsigned int GetUniformLocation(const std::string& name);
        ShaderProgramSource ParseShader(const std::string& filepath);
        unsigned int CompileShader(unsigned int type, const std::string& source);
        unsigned int CreateProgram(const std::string& vertexShader, const std::string& fragmentShader);

    public:
        Shader(const std::string& filepath);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetUniform1f(const std::string& name, float f0);
        void SetUniform2f(const std::string& name, float f0, float f1);
        void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
        void SetUniform2fv(const std::string& name, const glm::vec2& vector);
        void SetUniform4fv(const std::string& name, const glm::vec4& vector);
        void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
};
