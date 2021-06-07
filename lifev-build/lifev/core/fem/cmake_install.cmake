# Install script for directory: /home/lifev/lifev-src/lifev/core/fem

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/lifev/core/fem" TYPE FILE FILES
    "/home/lifev/lifev-src/lifev/core/fem/Assembly.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/AssemblyElemental.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCBase.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCDataInterpolator.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCFunction.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCHandler.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCIdentifier.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCManage.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCManageNormal.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/BCVector.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/CurrentFE.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/CurrentFEManifold.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOF.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFDiscontinuous.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFExtended.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFGatherer.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFInterface.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFInterface3Dto2D.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFInterface3Dto3D.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/DOFLocalPattern.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FEField.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FEFunction.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FESpace.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FESpaceExtended.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FESpaceDiscontinuous.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/FESpaceInterpolation.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/GeometricMap.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/GradientRecovery.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/HyperbolicFluxNumerical.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/QRKeast.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/PostProcessingBoundary.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/QRKeast.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/QuadraturePoint.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/QuadratureRule.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/QuadratureRuleProvider.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/ReferenceElement.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/ReferenceFE.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/ReferenceFEHdiv.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/ReferenceFEHybrid.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/ReferenceFEScalar.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/SobolevNorms.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeAdvance.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeAdvanceBDF.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeAdvanceBDFVariableStep.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeAdvanceData.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeAdvanceNewmark.hpp"
    "/home/lifev/lifev-src/lifev/core/fem/TimeData.hpp"
    )
endif()

