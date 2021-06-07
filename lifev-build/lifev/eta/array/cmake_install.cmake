# Install script for directory: /home/lifev/lifev-src/lifev/eta/array

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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "ETA")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/eta/array" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/eta/array/ETMatrixElemental.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/ETVectorElemental.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallAddition.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallArcTan.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallCofactor.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallCubicRoot.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallDerivativeArcTan.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallDeterminant.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallDivision.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallDot.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallEmult.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallExponential.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallExtract.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallInverse.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallLogarithm.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallMinMax.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallMinusTranspose.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallNormalize.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallOuterProduct.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallPower.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallProduct.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallSquareRoot.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallSubstraction.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallSymmetricTensor.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallTrace.hpp"
    "/home/lifev/lifev-src/lifev/eta/array/OperationSmallTranspose.hpp"
    )
endif()

