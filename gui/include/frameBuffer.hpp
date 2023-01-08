// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "helpers.hpp"
#include "log.hpp"
#include <cstdint>
#include <glad/glad.h>
#include <ostream>
#include <stdexcept>

namespace cgns_tools::gui
{

struct frameBuffer
{

  frameBuffer(const int32_t width = 800, const int32_t height = 600)
    : _fbo{ 0 }
    , _textureId{ 0 }
    , _renderBufferId{ 0 }
    , _width{ width }
    , _height{ height }
  {
    create_buffers();
  }

  ~frameBuffer() { delete_buffers(); }

  auto get_texture() { return _textureId; }

  void unbind() const { opengl_fn<glBindFramebuffer>(GL_FRAMEBUFFER, 0); }

  void bind()
  {
    opengl_fn<glBindFramebuffer>(GL_FRAMEBUFFER, _fbo);
    opengl_fn<glViewport>(0, 0, _width, _height);
    opengl_fn<glClear>(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  bool fits(const int32_t width, const int32_t height) const
  {
    return _width == width && _height == height;
  }

  void resize(const int32_t width, const int32_t height)
  {
    delete_buffers();
    _width = width;
    _height = height;
    create_buffers();
  }

private:
  uint32_t _fbo;
  uint32_t _textureId;
  uint32_t _renderBufferId;
  int32_t _width;
  int32_t _height;

private:
  void create_buffers()
  {
    if (_fbo)
    {
      delete_buffers();
    }

    opengl_fn<glGenFramebuffers>(1, &_fbo);
    opengl_fn<glBindFramebuffer>(GL_FRAMEBUFFER, _fbo);

    // create color texture
    opengl_fn<glGenTextures>(1, &_textureId);
    opengl_fn<glBindTexture>(GL_TEXTURE_2D, _textureId);
    opengl_fn<glTexImage2D>(GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            _width,
                            _height,
                            0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            nullptr);
    opengl_fn<glTexParameteri>(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    opengl_fn<glTexParameteri>(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    opengl_fn<glBindTexture>(GL_TEXTURE_2D, 0);
    opengl_fn<glFramebufferTexture2D>(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);

    // create depth & stencil renderbuffer
    opengl_fn<glGenRenderbuffers>(1, &_renderBufferId);
    opengl_fn<glBindRenderbuffer>(GL_RENDERBUFFER, _renderBufferId);
    opengl_fn<glRenderbufferStorage>(
      GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    opengl_fn<glBindRenderbuffer>(GL_RENDERBUFFER, 0);
    opengl_fn<glFramebufferRenderbuffer>(GL_FRAMEBUFFER,
                                         GL_DEPTH_STENCIL_ATTACHMENT,
                                         GL_RENDERBUFFER,
                                         _renderBufferId);

    // check for completeness
    int32_t completeStatus =
      opengl_fn<glCheckFramebufferStatus>(GL_FRAMEBUFFER);
    if (completeStatus != GL_FRAMEBUFFER_COMPLETE)
    {
      log_error("Failure to create framebuffer. Complete status: {}",
                completeStatus);

      throw std::runtime_error("OpenGL error encountered.");
    }

    unbind();
  }

  void delete_buffers()
  {
    if (_fbo)
    {
      opengl_fn<glDeleteFramebuffers>(1, &_fbo);
      // opengl_fn<glDeleteTextures>(1, &_textureId);
      // opengl_fn<glDeleteRenderbuffers>(1, &_renderBufferId);
      _fbo = 0;
      _textureId = 0;
      _renderBufferId = 0;
    }
  }
};

} // namespace cgns_tools::gui