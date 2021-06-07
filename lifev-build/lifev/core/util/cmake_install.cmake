# Install script for directory: /home/lifev/lifev-src/lifev/core/util

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/util" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/util/FactoryTypeInfo.hpp"
    "/home/lifev/lifev-src/lifev/core/util/Switch.hpp"
    "/home/lifev/lifev-src/lifev/core/util/EncoderBase64.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeDebug.hpp"
    "/home/lifev/lifev-src/lifev/core/util/StringUtility.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeAssertSmart.hpp"
    "/home/lifev/lifev-src/lifev/core/util/Parser.hpp"
    "/home/lifev/lifev-src/lifev/core/util/FactorySingleton.hpp"
    "/home/lifev/lifev-src/lifev/core/util/StringData.hpp"
    "/home/lifev/lifev-src/lifev/core/util/ParserSpiritGrammar.hpp"
    "/home/lifev/lifev-src/lifev/core/util/FortranWrapper.hpp"
    "/home/lifev/lifev-src/lifev/core/util/Factory.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeAssert.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeChrono.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeChronoManager.hpp"
    "/home/lifev/lifev-src/lifev/core/util/Displayer.hpp"
    "/home/lifev/lifev-src/lifev/core/util/ParserDefinitions.hpp"
    "/home/lifev/lifev-src/lifev/core/util/FactoryPolicy.hpp"
    "/home/lifev/lifev-src/lifev/core/util/WallClock.hpp"
    "/home/lifev/lifev-src/lifev/core/util/OpenMPParameters.hpp"
    "/home/lifev/lifev-src/lifev/core/util/VerifySolutions.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeTraits.hpp"
    "/home/lifev/lifev-src/lifev/core/util/LifeTolerance.hpp"
    "/home/lifev/lifev-src/lifev/core/util/ConditionalOstream.hpp"
    "/home/lifev/lifev-src/lifev/core/util/Flag.hpp"
    )
endif()

