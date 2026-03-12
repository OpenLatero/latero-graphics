cmake_minimum_required (VERSION 3.10...3.31)
project (latero-graphics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# check for dependencies using pkg-config
include(FindPkgConfig)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(LIBXML REQUIRED IMPORTED_TARGET libxml++-3.0)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost CONFIG COMPONENTS system filesystem thread REQUIRED)
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
  ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} doxygen.cfg)
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

# Treat deprecation in GTKMM as errors
#target_compile_definitions(latero-gui PRIVATE
#    GLIB_DISABLE_DEPRECATED
#    GDK_DISABLE_DEPRECATED
#    GTK_DISABLE_DEPRECATED
#    GTKMM_DISABLE_DEPRECATED
#)