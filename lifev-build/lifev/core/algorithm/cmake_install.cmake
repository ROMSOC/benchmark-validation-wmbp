# Install script for directory: /home/lifev/lifev-src/lifev/core/algorithm

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/algorithm" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/algorithm/SolverAmesos.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerComposed.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/NonLinearLineSearch.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerAztecOO.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/NonLinearRichardson.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/ComposedOperator.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/SolverAztecOO.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/NonLinearAitken.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerIfpack.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/NonLinearBrent.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/Preconditioner.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/EigenSolver.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/LinearSolver.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerML.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerBlock.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerComposition.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerTeko.hpp"
    "/home/lifev/lifev-src/lifev/core/algorithm/PreconditionerLinearSolver.hpp"
    )
endif()

