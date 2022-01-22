ADD_LIBRARY(platform_utils
  tiny_chrome_trace_util.cpp
  tiny_chrome_trace_util.h
  tiny_clock.cpp
  tiny_clock.h
  tiny_logging.cpp
  tiny_logging.h
)

target_include_directories(platform_utils PUBLIC  . )

