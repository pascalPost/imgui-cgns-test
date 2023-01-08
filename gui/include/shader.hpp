// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "helpers.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>

namespace cgns_tools::gui
{

struct shader
{
  shader() { compile(); }

  ~shader() { opengl_fn<glDeleteProgram>(_shaderProgram); }

  void use() const { opengl_fn<glUseProgram>(_shaderProgram); }

  // void set_mat4(const glm::mat4& mat4, const std::string& name)
  // {
  //   GLint myLoc = glGetUniformLocation(_shaderProgram, name.c_str());
  //   glUniformMatrix4fv(myLoc, 1, GL_FALSE, glm::value_ptr(mat4));
  // }

  // void set_vec3(const glm::vec3& vec3, const std::string& name)
  // {
  //   GLint myLoc = glGetUniformLocation(_shaderProgram, name.c_str());
  //   glProgramUniform3fv(_shaderProgram, myLoc, 1, glm::value_ptr(vec3));
  // }

  // void set_f1(float v, const std::string& name)
  // {
  //   GLint myLoc = glGetUniformLocation(_shaderProgram, name.c_str());
  //   glUniform1f(myLoc, v);
  // }

private:
  uint32_t _shaderProgram;

  // const char* vertexShaderSource =
  //   "#version 330 core\n"
  //   "layout (location = 0) in vec3 aPosition;\n"
  //   "\n"
  //   // "uniform mat4 model;\n"
  //   // "uniform mat4 view;\n"
  //   // "uniform mat4 projection;\n"
  //   "\n"
  //   "void main()\n"
  //   "{\n"
  //   // "   gl_Position = projection * view * model *
  //   vec4(aPosition, 1.0f);\n" "   gl_Position = vec4(aPosition, 1.0f);\n"
  //   "}\n";

  const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

  const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

  void compile()
  {
    int success;
    char infoLog[512];

    // vertex shader
    const auto vertexShader = opengl_fn<glCreateShader>(GL_VERTEX_SHADER);
    opengl_fn<glShaderSource>(vertexShader, 1, &vertexShaderSource, nullptr);
    opengl_fn<glCompileShader>(vertexShader);
    opengl_fn<glGetShaderiv>(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      opengl_fn<glGetShaderInfoLog>(vertexShader, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader;
    fragmentShader = opengl_fn<glCreateShader>(GL_FRAGMENT_SHADER);
    opengl_fn<glShaderSource>(
      fragmentShader, 1, &fragmentShaderSource, nullptr);
    opengl_fn<glCompileShader>(fragmentShader);
    opengl_fn<glGetShaderiv>(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      opengl_fn<glGetShaderInfoLog>(fragmentShader, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    // link shaders to shader program
    _shaderProgram = opengl_fn<glCreateProgram>();
    opengl_fn<glAttachShader>(_shaderProgram, vertexShader);
    opengl_fn<glAttachShader>(_shaderProgram, fragmentShader);
    opengl_fn<glLinkProgram>(_shaderProgram);
    opengl_fn<glGetProgramiv>(_shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      opengl_fn<glGetProgramInfoLog>(_shaderProgram, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }

    opengl_fn<glDeleteShader>(vertexShader);
    opengl_fn<glDeleteShader>(fragmentShader);
  }
};

} // namespace cgns_tools::gui