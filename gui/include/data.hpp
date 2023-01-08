// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "shader.hpp"
#include "vertexBuffer.hpp"
#include <array>
#include <cgns-tools.hpp>
#include <glm/glm.hpp>
#include <optional>

namespace cgns_tools::gui
{

struct data
{

  // data()
  //   : _color()
  //   , _roughness()
  //   , _metallic()
  //   , _file()
  //   , _data()
  //   , _vertexBuffer()
  // {
  //   std::vector<float> vertices;

  //   vertices.reserve(9);

  //   // p0
  //   vertices.emplace_back(-0.5);
  //   vertices.emplace_back(-0.5);
  //   vertices.emplace_back(0.0);

  //   // p1
  //   vertices.emplace_back(0.5);
  //   vertices.emplace_back(-0.5);
  //   vertices.emplace_back(0.0);

  //   // p2
  //   vertices.emplace_back(0.0);
  //   vertices.emplace_back(0.5);
  //   vertices.emplace_back(0.0);

  //   _vertexBuffer = cgns_tools::gui::vertexBuffer{ std::move(vertices) };
  // }

  data(const glm::vec3 color = { 1.0, 0.0, 0.0 },
       const float roughness = 0.2,
       const float metallic = 0.1)
    : _color{ color }
    , _roughness{ roughness }
    , _metallic{ metallic }
    , _file{}
    , _data{}
    , _vertexBuffer{}
  {
  }

  void loadFile(const std::string& path)
  {
    _file = path;

    cgns_tools::fileIn f{ _file };
    _data = cgns_tools::root{ f.readBaseInformation() };

    const auto& base = (*_data).bases[0];

    const auto& zone = std::get<cgns_tools::zoneStructured>(base.zones[0]);

    const auto& gridCoordinates = zone.gridCoordinates[0];

    assert(gridCoordinates.dataArrays.size() == 3);

    std::vector<float> vertices;

    const unsigned nPoints =
      std::visit([](auto&& dataArray) { return dataArray.data.size(); },
                 gridCoordinates.dataArrays[0]);

    // std::array<float, 3> min{ std::numeric_limits<float>::max(),
    //                           std::numeric_limits<float>::max(),
    //                           std::numeric_limits<float>::max() };
    // std::array<float, 3> max{ std::numeric_limits<float>::min(),
    //                           std::numeric_limits<float>::min(),
    //                           std::numeric_limits<float>::min() };

    for (unsigned iVert = 0; iVert < nPoints; ++iVert)
    {
      for (unsigned iCoord = 0; iCoord < gridCoordinates.dataArrays.size();
           ++iCoord)
      {
        float x = std::visit([iVert](const auto& dataArray) -> float
                             { return dataArray.data[iVert]; },
                             gridCoordinates.dataArrays[iCoord]);

        if (iCoord == 0)
        {
          x /= 10;
        }
        else if (iCoord == 1)
        {
          x *= 10;
        }
        else if (iCoord == 2)
        {
          x /= 10;
        }

        // if (x < min[iCoord])
        // {
        //   min[iCoord] = x;
        // }

        // if (x > max[iCoord])
        // {
        //   max[iCoord] = x;
        // }

        vertices.emplace_back(x);
      }
    }

    _vertexBuffer = cgns_tools::gui::vertexBuffer{ std::move(vertices) };
  }

  void update(shader& shader)
  {
    // shader.set_vec3(_color, "albedo");
    // shader.set_f1(_roughness, "roughness");
    // shader.set_f1(_metallic, "metallic");
    // shader.set_f1(1.0f, "ao");
  }

  // operator bool() { return _data.has_value(); }
  operator bool() { return _vertexBuffer.has_value(); }

  void render(const shader& shader)
  {
    if (_vertexBuffer)
    {
      _vertexBuffer->draw(shader);
    }
  }

  const auto& file() noexcept { return _file; }

  const auto& operator()() { return _data; }

private:
  glm::vec3 _color;
  float _roughness;
  float _metallic;

  std::string _file;
  std::optional<root> _data;
  std::optional<vertexBuffer> _vertexBuffer;
};

} // namespace cgns_tools::gui