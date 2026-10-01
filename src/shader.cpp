/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Implementation file for shader. This class handles compilation of shaders and updating uniforms.
*/
#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const char* vertPath, const char* fragPath)
{
    std::string vertSource = readFile(vertPath);
    std::string fragSource = readFile(fragPath);

    if (vertSource.empty() || fragSource.empty())
    {
        std::cerr << "[Shader] one of the source files is empty or not found\n";
        throw std::runtime_error("Shaders not found. Run from same dir as exe\n");
    }
    
    GLuint vertHandle = compile(GL_VERTEX_SHADER, vertSource.c_str(), "Vertex");
    GLuint fragHandle = compile(GL_FRAGMENT_SHADER, fragSource.c_str(), "Fragment");
    
    id = glCreateProgram();
    glAttachShader(id, vertHandle);
    glAttachShader(id, fragHandle);
    glLinkProgram(id);
    checkLink(id);
    
    glDeleteShader(vertHandle);
    glDeleteShader(fragHandle);
    
}



//Added some error checking here
GLint Shader::loc(const std::string& n) const
{ 
    GLint loc = glGetUniformLocation(id, n.c_str());
    if (loc == -1) 
    {
        std::cout << "Failed to find a uniform " << n << std::endl;
    } 
    return loc;
}

// ── readFile (provided) ───────────────────────────────────────────────────
//puts file into a stringstream and returns it's string
std::string Shader::readFile(const char* path)
{
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "[Shader] Cannot open: " << path << "\n";
        return {};
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


GLuint Shader::compile(GLenum type, const char* src, const char* label)
{
    GLuint result = glCreateShader(type);
    glShaderSource(result, 1, &src, nullptr);
    glCompileShader(result);

    int success;
    char infoLog[512];

    glGetShaderiv(result, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(result, 512, nullptr, infoLog);
        std::cout << label << "[Shader] shader failed to compile. More info:\n" << infoLog << "\n";
    }
    
    return result;
}

void Shader::checkLink(GLuint prog)
{
    int success;
    char infoLog[512];
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cout << "[Shader] Shader program failed to link. More info:\n" << infoLog << "\n";
    }
}

