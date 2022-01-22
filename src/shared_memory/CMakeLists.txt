
if (WIN32)
  ADD_LIBRARY(shared_memory 
    win32_shared_memory.cpp 
    win32_shared_memory.h
    shared_memory_interface.h
    )
else()
  ADD_LIBRARY(shared_memory 
    posix_shared_memory.cpp 
    posix_shared_memory.h
    shared_memory_interface.h
    )

  if (UNIX AND NOT APPLE)
    target_link_libraries(shared_memory ${DL})
  elseif (APPLE)
    find_library(COCOA NAMES Cocoa)
    MESSAGE(${COCOA})
  endif ()
endif()

target_include_directories(shared_memory PUBLIC  . )

