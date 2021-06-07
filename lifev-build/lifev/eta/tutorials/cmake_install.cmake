# Install script for directory: /home/lifev/lifev-src/lifev/eta/tutorials

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/lifev/lifev-build/lifev/eta/tutorials/1_ETA_laplacian/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/2_ETA_ADR/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/3_ETA_rhs_and_value/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/4_ETA_vectorial_laplacian/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/5_ETA_debug_expressions/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/6_ETA_functor/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/7_ETA_blocks/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/8_ETA_block_manip/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/9_ETA_QRProvider/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/10_ETA_QR_Advanced/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/9_ETA_gradient_interpolation/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/11_ETA_LaplacianPhiI/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/12_ETA_LaplacianPhiJ/cmake_install.cmake")
  include("/home/lifev/lifev-build/lifev/eta/tutorials/13_ETA_LaplacianVector/cmake_install.cmake")

endif()

