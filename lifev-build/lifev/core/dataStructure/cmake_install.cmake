# Install script for directory: /home/lifev/lifev-src/lifev/core/dataStructure

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/dataStructure" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/dataStructure/ADTree.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/BoundingBox.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/Collection.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/Domain.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/ExceptionHandling.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/Header.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/LinearTransformation.hpp"
    "/home/lifev/lifev-src/lifev/core/dataStructure/TreeNode.hpp"
    )
endif()

