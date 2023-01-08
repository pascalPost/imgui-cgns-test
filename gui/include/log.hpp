// Copyright (c) Pascal Post. All Rights Reserved.
// Licensed under AGPLv3 license (see LICENSE.txt for details)

#pragma once

#include <spdlog/spdlog.h>

namespace cgns_tools::gui
{

static constexpr auto default_logger_name = "cgns-tools-gui-logger";

template<typename... Args>
void
log_error(spdlog::format_string_t<Args...> fmt, Args&&... args)
{
  if (spdlog::get(default_logger_name) != nullptr)
  {
    spdlog::get(default_logger_name)->error(std::forward<Args>(args)...);
  }
}

} // namespace cgns_tools::gui