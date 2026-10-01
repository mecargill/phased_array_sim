/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Header file for shader. This class handles compilation of shaders and updating uniforms.
*/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

//The shader class handles compiling and linking shaders
//It also handles updating uniforms
class Shader
{
public:
    GLuint id { 0 };

    /**
	* Default constructor. Does not initialize the shader, so the id will be 0 and using it will cause an error.
    *
    */
    Shader() {}

    /**
	* Constructor that takes paths to the vertex and fragment shader source files. 
    * It compiles the shaders, links them into a program, and stores the program ID in the id member variable. 
    * If any step fails, it prints an error message and throws a runtime_error.
    *
    */
    Shader(const char* vertPath, const char* fragPath);
    
    void use() const { glUseProgram(id); }

   
    void setBool (const std::string& n, bool  v) const { glUniform1i (loc(n), (int)v); }
    void setInt  (const std::string& n, int   v) const { glUniform1i (loc(n), v); }
    void setFloat(const std::string& n, float v) const { glUniform1f (loc(n), v); }

    void setVec3(const std::string& n, const glm::vec3& v) const
        { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }

    void setVec3(const std::string& n, float x, float y, float z) const
        { glUniform3f(loc(n), x, y, z); }

    void setMat4(const std::string& n, const glm::mat4& m) const
        { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(m)); }

private:
    //Added some error checking here
    GLint loc(const std::string& n) const;
   
    // ── readFile ───────────────────────────────────────────────────
    //puts file into a stringstream and returns it's string
    std::string readFile(const char* path);
   
    /**
	* Compiles a shader of the given type from the given source code. If compilation fails, prints an error message and throws a runtime_error.
    *
    */
    GLuint compile(GLenum type, const char* src, const char* label);
    

    /**
	* Checks if the given program linked successfully. If not, prints an error message and throws a runtime_error.
    *
    */
    void checkLink(GLuint prog);
    
};
