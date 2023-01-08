// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "log.hpp"
#include <glad/glad.h>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace cgns_tools::gui
{

/// OpenGL error checking function
/// @todo add option to deactivate via macro
inline void
opengl_error_check()
{
  // #ifndef NDEBUG
  GLenum error = glGetError();
  while (error != GL_NO_ERROR)
  {
    std::string errorstr;
    switch (error)
    {
      case GL_INVALID_OPERATION:
        errorstr = "INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        errorstr = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        errorstr = "GL_INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        errorstr = "GL_OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        errorstr = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      default:
        errorstr = std::to_string(error);
        break;
    }

    log_error("OpenGL Error: {}", errorstr.c_str());
    throw std::runtime_error("OpenGL error encountered.");

    error = glGetError();
  }
  // #endif
}

// /// OpenGL function call with error handling
// template<auto& F, class... Args>
// void
// opengl_fn(Args&&... args)
// {
//   F(args...);
//   opengl_error_check();
// }

/// OpenGL function call with error handling
template<auto& F, class... Args>
auto
opengl_fn(Args&&... args) -> decltype(F(args...))
{
  if constexpr (std::is_same_v<decltype(F(args...)), void>)
  {
    F(args...);
    opengl_error_check();
  }
  else
  {
    const auto res = F(args...);
    opengl_error_check();

    return res;
  }
}

} // namespace cgns_tools::gui