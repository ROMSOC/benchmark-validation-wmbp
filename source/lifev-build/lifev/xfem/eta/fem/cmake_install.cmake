# Install script for directory: /home/lifev/lifev-src/lifev/xfem/eta/fem

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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "XFEM")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/xfem/eta/fem" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/ConstitutiveLawsMCI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/Materials.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/ExpressionDefinitionsMCI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/Hooke.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/SecondOrderExpHolzGen.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/VenantKirchhoff.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/CouplingFSI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/CouplingTypes.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/NoSlip.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/Slip.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/eta/fem/SlipContactPlane.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/ConstitutiveLawsMCI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/Materials.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/ExpressionDefinitionsMCI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/Hooke.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/SecondOrderExpHolzGen.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/VenantKirchhoff.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/CouplingFSI.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/CouplingTypes.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/NoSlip.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/Slip.hpp"
    "/home/lifev/lifev-src/lifev/xfem/eta/fem/SlipContactPlane.hpp"
    )
endif()

