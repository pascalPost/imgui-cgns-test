// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include "shader.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/vec2.hpp>

namespace cgns_tools::gui
{

struct camera
{
  camera(const glm::vec3& position,
         float fov,
         float aspect,
         float near,
         float far)
  {
    mPosition = position;
    mNear = near;
    mFar = far;
    mFOV = fov;

    set_aspect(aspect);

    update_view_matrix();
  }

  void update(cgns_tools::gui::shader& shader)
  {
    glm::mat4 model{ 1.0f };
    shader.set_mat4(model, "model");
    shader.set_mat4(mViewMatrix, "view");
    shader.set_mat4(get_projection(), "projection");
    shader.set_vec3(mPosition, "camPos");
  }

  void set_aspect(float aspect)
  {
    mProjection = glm::perspective(mFOV, aspect, mNear, mFar);
  }

  void update_view_matrix()
  {
    mPosition = mFocus - get_forward() * mDistance;

    glm::quat orientation = get_direction();
    mViewMatrix =
      glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(orientation);
    mViewMatrix = glm::inverse(mViewMatrix);
  }

  glm::quat get_direction() const
  {
    return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
  }

  glm::vec3 get_forward() const
  {
    return glm::rotate(get_direction(), cForward);
  }

  const glm::mat4& get_projection() const { return mProjection; }

private:
  glm::mat4 mViewMatrix;
  glm::mat4 mProjection = glm::mat4{ 1.0f };
  glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };

  glm::vec3 mFocus = { 0.0f, 0.0f, 0.0f };

  float mDistance = 5.0f;

  float mFOV;
  float mNear;
  float mFar;

  float mPitch = 0.0f;
  float mYaw = 0.0f;

  const glm::vec3 cForward = { 0.0f, 0.0f, -1.0f };
};

} // namespace cgns_tools::gui