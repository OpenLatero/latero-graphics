# Install script for directory: /Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/laterographics" TYPE FILE FILES
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audioengine.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audioenginefwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/averagefilter.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/buttonevent.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/engine.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/external.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/generator.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/generatorfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/graph.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/grating.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratinggraph.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulator.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/logger.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mainpage.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mask.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/maskfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/maskwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/modifiable.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mutex.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/namedid.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillator.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/point.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/pointlistwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/pointwidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/positiongen.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/positiongenfwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridge.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgefwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgegraph.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgewidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/state.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/tactileengine.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/tactileenginefwd.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/timer.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/units.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/vector.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/velocityestimator.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/virtualsurfacewidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/visualizewidget.h"
    "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/xml.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/liblaterographics.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gtk/cmake_install.cmake")
  include("/Users/vleves/Documents/Git/OpenLatero/latero-graphics/latero-graphics/graphics/cmake_install.cmake")

endif()

