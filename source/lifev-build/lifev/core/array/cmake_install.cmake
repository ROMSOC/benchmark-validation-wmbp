# Install script for directory: /home/lifev/lifev-src/lifev/core/array

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/array" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/array/EnumMapEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MapVector.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorSmall.hpp"
    "/home/lifev/lifev-src/lifev/core/array/RNMTemplate.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorBlockStructure.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixBlockStructure.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixEpetraStructuredUtility.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixEpetraStructuredView.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MapEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MapEpetraData.hpp"
    "/home/lifev/lifev-src/lifev/core/array/RNMOperatorConstant.hpp"
    "/home/lifev/lifev-src/lifev/core/array/RNM.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorContainer.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixElemental.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorEpetraStructured.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixEpetraStructured.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixBlockMonolithicEpetraView.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixBlockMonolithicEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixSmall.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorBlockMonolithicEpetra.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorBlockMonolithicEpetraView.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorEpetraStructuredView.hpp"
    "/home/lifev/lifev-src/lifev/core/array/ArraySimple.hpp"
    "/home/lifev/lifev-src/lifev/core/array/VectorElemental.hpp"
    "/home/lifev/lifev-src/lifev/core/array/RNMOperator.hpp"
    "/home/lifev/lifev-src/lifev/core/array/MatrixContainer.hpp"
    "/home/lifev/lifev-src/lifev/core/array/GhostHandler.hpp"
    "/home/lifev/lifev-src/lifev/core/array/CoordinateSystem.hpp"
    )
endif()

