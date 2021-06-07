# Install script for directory: /home/lifev/lifev-src/lifev/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/lifev/lifev-install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Core")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/LifeV.hpp"
    "/home/lifev/lifev-src/lifev/core/LifeVersion.hpp"
    "/home/lifev/lifev-build/lifev/core/Core_config.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Core")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/lifev/lifev-build/lifev/core/liblifevcore.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Core" TYPE FILE RENAME "CoreConfig.cmake" FILES "/home/lifev/lifev-build/lifev/core/CMakeFiles/CoreConfig_install.cmake")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE RENAME "Makefile.export.Core" FILES "/home/lifev/lifev-build/lifev/core/CMakeFiles/Makefile.export.Core_install")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/lifev/lifev-build/lifev/core/algorithm/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/array/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/dataStructure/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/fem/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/filter/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/function/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/mesh/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/operator/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/solver/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/core/util/cmake_install.cmake")

endif()

