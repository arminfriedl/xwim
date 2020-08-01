#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <cstdlib>

#ifdef NDEBUG
#define XWIM_LOGLEVEL SPDLOG_LEVEL_ERROR
#else
#define XWIM_LOGLEVEL SPDLOG_LEVEL_DEBUG
#endif

namespace xwim {
namespace log {

/**
 * Get log level from XWIM_LOGLEVEL environment variable.
 * For valid values see SPDLOG_LEVEL_NAMES in spdlog/common.h
 *
 * @returns spdlog::level::level_enum::off if no valid XWIM_LOGLEVEL defined
 */
spdlog::level::level_enum _init_from_env() {
  char* env_lvl = std::getenv("XWIM_LOGLEVEL");
  if (!env_lvl) {
    return spdlog::level::level_enum::off;
  }

  spdlog::level::level_enum lvl = spdlog::level::from_str(env_lvl);

  //`::from_str` returns `off` if no match found
  if (spdlog::level::level_enum::off == lvl) {
    spdlog::debug("No environment definition for log level");  // uses default
                                                               // logger/level
  }

  return lvl;
}

/**
 * Get log level from compile time definition.
 *
 * @return spdlog::level::level_enum::error for release builds (-DNDEBUG)
 *         spdlog::level::level_enum::debug for debug builds
 */
spdlog::level::level_enum _init_from_compile() {
  return static_cast<spdlog::level::level_enum>(XWIM_LOGLEVEL);
}

/**
 * Determine the log level from various sources at runtime.
 *
 * The log level is determined from sources in the following order (first
 * wins):
 * 1. The `level` argument
 * 2. The XWIM_LOGLEVEL environment variable
 * 3. The default for the build type (-DNDEBUG)
 *      -> ERROR for release builds
 *      -> DEBUG for debug builds
 *
 * The determined level is then set for the default logger via
 * `spdlog::set_level`.
 */
void init(spdlog::level::level_enum level = spdlog::level::level_enum::off) {
  if (spdlog::level::level_enum::off != level) {
    spdlog::set_level(level);
    return;
  }

  level = _init_from_env();
  if (spdlog::level::level_enum::off != level) {
    spdlog::set_level(level);
    return;
  }

  return spdlog::set_level(_init_from_compile());
}

}  // namespace log
}  // namespace xwim
