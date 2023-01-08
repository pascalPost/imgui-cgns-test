// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "helpers.hpp"
#include "shader.hpp"
#include <glad/glad.h>
#include <type_traits>
#include <utility>
#include <vector>

namespace cgns_tools::gui
{

struct vertexBuffer
{

  /// constructor
  vertexBuffer(std::vector<float>&& vertices)
    : _vertices{ std::move(vertices) }
    , _vbo{}
    , _vao{}
  {
    create_buffers();
  }

  /// destructor
  ~vertexBuffer() { delete_buffers(); }

  /// copy constructor
  vertexBuffer(const vertexBuffer& other) = delete;

  /// move constructor
  vertexBuffer(vertexBuffer&& other) noexcept
    : _vertices(std::move(other._vertices))
    , _vbo{ other._vbo }
    , _vao{ other._vao }
  {
    other._vbo = 0;
    other._vao = 0;
  }

  /// copy assignment
  vertexBuffer& operator=(const vertexBuffer& other) = delete;

  /// move assignment
  vertexBuffer& operator=(vertexBuffer&& other) noexcept
  {
    std::swap(_vertices, other._vertices);
    std::swap(_vbo, other._vbo);
    std::swap(_vao, other._vao);
    return *this;
  }

  void draw(const shader& shader)
  {
    shader.use();

    bind();

    opengl_fn<glPointSize>(2);
    opengl_fn<glDrawArrays>(GL_POINTS, 0, _vertices.size() / 3);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    unbind();
  }

private:
  std::vector<float> _vertices;

  GLuint _vbo;
  GLuint _vao;
  //   GLuint _IBO;

  void bind() { opengl_fn<glBindVertexArray>(_vao); }

  void unbind() { opengl_fn<glBindVertexArray>(0); }

  void create_buffers()
  {
    opengl_fn<glGenVertexArrays>(1, &_vao);
    opengl_fn<glBindVertexArray>(_vao);

    opengl_fn<glGenBuffers>(1, &_vbo);
    opengl_fn<glBindBuffer>(GL_ARRAY_BUFFER, _vbo);
    opengl_fn<glBufferData>(GL_ARRAY_BUFFER,
                            sizeof(float) * _vertices.size(),
                            _vertices.data(),
                            GL_STATIC_DRAW);

    opengl_fn<glVertexAttribPointer>(
      0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    opengl_fn<glEnableVertexAttribArray>(0);
  }

  void delete_buffers()
  {
    if (_vbo)
    {
      opengl_fn<glDeleteBuffers>(1, &_vbo);
    }

    if (_vao)
    {
      opengl_fn<glDeleteVertexArrays>(1, &_vao);
    }
  }
};

} // namespace cgns_tools::gui