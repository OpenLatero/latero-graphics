cmake_minimum_required (VERSION 2.4)
project (latero-graphics)

# add /usr/loca/lib/pkgconfig (necessary on macOS)
set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig" )

# check for dependencies using pkg-config
include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED latero ImageMagick++ gtkmm-2.4 libxml++-2.6)

FIND_PROGRAM(AUDIERE_CONFIG audiere-config)
IF(AUDIERE_CONFIG)
  EXEC_PROGRAM(${AUDIERE_CONFIG}
     ARGS --cxxflags
     OUTPUT_VARIABLE AUDIERE_CFLAGS)
  EXEC_PROGRAM(${AUDIERE_CONFIG}
     ARGS --libs
     OUTPUT_VARIABLE AUDIERE_LDFLAGS)
ELSE(AUDIERE_CONFIG)
  MESSAGE(STATUS "Cannot find audiere-config anywhere on the system.")
ENDIF(AUDIERE_CONFIG)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./")
#include(FindXenomai)
#find_package (Xenomai REQUIRED)

set(DEPS_LIBRARY_DIRS ${DEPS_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS} ${XENOMAI_LIBRARY_DIRS})
set(DEPS_INCLUDE_DIRS ${DEPS_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${XENOMAI_INCLUDE_DIRS})
set(DEPS_LDFLAGS ${DEPS_LDFLAGS} ${AUDIERE_LDFLAGS} -l${Boost_SYSTEM_LIBRARY} -l${Boost_FILESYSTEM_LIBRARY} -l${Boost_THREAD_LIBRARY} ${XENOMAI_C_CONFIG_LIBS})
set(DEPS_CFLAGS ${DEPS_CFLAGS} ${AUDIERE_CFLAGS} ${XENOMAI_C_CONFIG_CFLAGS})

link_directories(${DEPS_LIBRARY_DIRS})
include_directories(${DEPS_INCLUDE_DIRS})


# todo: The DEPS_LDFLAGS and DEPS_CFLAGS strings separate the arguments with semicolons, which break
# when passed to latero.pc. This replaces them with spaces. There must be a better way of doing this.
STRING(REPLACE ";" " " DEPS_LDFLAGS_WSPACE "${DEPS_LDFLAGS}" )
STRING(REPLACE ";" " " DEPS_CFLAGS_WSPACE "${DEPS_CFLAGS}" )

# process subdirectories
add_subdirectory (latero-graphics)
add_subdirectory (media)

# generate latero.pc file and install it for pkg-config
configure_file(latero-graphics.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/latero-graphics.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/latero-graphics.pc DESTINATION lib/pkgconfig)

# generate doc if Doxygen is found
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_CUSTOM_TARGET(doc ${DOXYGEN_EXECUTABLE} doxygen.cfg)
ENDIF(DOXYGEN_FOUND)
