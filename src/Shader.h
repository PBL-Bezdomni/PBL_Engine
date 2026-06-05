#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader();
	void AddComputeShader(const char* computePath);
    // use/activate the shader
    void Use();
    // delete program
    void DeleteProgram();
    // utility uniform functions
     // ------------------------------------------------------------------------
    void SetBool(const std::string& name, bool value) const;
    // ------------------------------------------------------------------------
    void SetInt(const std::string& name, int value) const;
    // ------------------------------------------------------------------------
    void SetFloat(const std::string& name, float value) const;
    // ------------------------------------------------------------------------
    void SetMat4(const std::string& name, glm::mat4 trans);
    // ------------------------------------------------------------------------
    void SetVec3(const std::string& name, glm::vec3 vector);
    // ------------------------------------------------------------------------
    void SetVec4(const std::string& name, glm::vec4 vector);
    // ------------------------------------------------------------------------
    void SetIVec4(const std::string& name, int x, int y, int z, int w) const;
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void CheckCompileErrors(unsigned int shader, std::string type);
};
#endif