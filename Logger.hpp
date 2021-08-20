#ifndef UTILITIES_CORE_LOGGER_HPP
#define UTILITIES_CORE_LOGGER_HPP

#include <fmt/format.h>

enum LogLevel
{
  Trace = -3,
  Debug = -2,
  Info = -1,
  Warn = 0,
  Error = 1,
  Fatal = 2
};

#define LOG_FREE(__level__, __channel__, __message__)                 \
  {                                                                   \
    std::stringstream ss1_;                                           \
    ss1_ << __message__;                                              \
    fmt::print("<{}> [{}] {}", __level__, __channel__, ss1_.str());   \
  }

#endif
