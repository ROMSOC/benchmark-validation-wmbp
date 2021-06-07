# Install script for directory: /home/lifev/lifev-src/lifev/core/filter

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/filter" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/filter/ExporterEnsight.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/MeshWriter.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterHDF5Mesh3D.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/GetPot.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/HDF5IO.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/Exporter.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ImporterMesh3D.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/Importer.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterEmpty.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterVTK.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterHDF5.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ImporterMesh2D.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ParserGmsh.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ParserINRIAMesh.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterHDF5Disc.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/ExporterHDF5XFEM.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/HDF5IO.hpp"
    "/home/lifev/lifev-src/lifev/core/filter/PartitionIO.hpp"
    )
endif()

