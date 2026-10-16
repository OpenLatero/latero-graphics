cmake_minimum_required (VERSION 3.10...3.31)
project (latero-graphics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# on Windows, install into the MSYS2 environment of the compiler (e.g. C:/msys64/ucrt64) by default,
# instead of C:/Program Files
if (WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	get_filename_component(MSYS2_PREFIX "${CMAKE_CXX_COMPILER}" DIRECTORY)
	get_filename_component(MSYS2_PREFIX "${MSYS2_PREFIX}" DIRECTORY)
	set(CMAKE_INSTALL_PREFIX "${MSYS2_PREFIX}" CACHE PATH "Install path prefix" FORCE)
endif()
include(GNUInstallDirs)

# install location of the stock textures (see latero-graphics/graphics/texture/stock.h)
set(LATERO_GRAPHICS_TX_DIR "${CMAKE_INSTALL_DATADIR}/laterographics/texture")

# check for dependencies using pkg-config
include(FindPkgConfig)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)
pkg_check_modules(LIBXML REQUIRED IMPORTED_TARGET libxml++-5.0)

find_package(latero REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

FIND_PROGRAM(AUDIERE_CONFIG audiere-config)
IF(AUDIERE_CONFIG)
  add_definitions(-DENABLE_AUDIERE)
  EXEC_PROGRAM(${AUDIERE_CONFIG}
     ARGS --cxxflags
     OUTPUT_VARIABLE AUDIERE_CFLAGS)
  EXEC_PROGRAM(${AUDIERE_CONFIG}
     ARGS --libs
     OUTPUT_VARIABLE AUDIERE_LDFLAGS)
ELSE(AUDIERE_CONFIG)
  MESSAGE(STATUS "Cannot find audiere-config anywhere on the system.")
ENDIF(AUDIERE_CONFIG)

set(DEPS_LDFLAGS ${DEPS_LDFLAGS} ${AUDIERE_LDFLAGS})
set(DEPS_CFLAGS ${DEPS_CFLAGS} ${AUDIERE_CFLAGS})

link_directories(${DEPS_LIBRARY_DIRS})
include_directories(${DEPS_INCLUDE_DIRS})

# process subdirectories
add_subdirectory (latero-graphics)
add_subdirectory (media)

# generate doc if Doxygen is found
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doxygen.cfg
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
ENDIF(DOXYGEN_FOUND)

# Create and install cmake config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/laterographicsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/laterographicsConfig.cmake
    INSTALL_DESTINATION lib/cmake/laterographics
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/laterographicsConfig.cmake
    DESTINATION lib/cmake/laterographics
)