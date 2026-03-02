# Install script for directory: /Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics

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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gtk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/graphics/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/laterographics" TYPE FILE FILES
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/audiodevice.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/audiodevicefwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/audioengine.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/audioenginefwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/averagefilter.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/buttonevent.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/engine.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/external.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/generator.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/generatorfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/graph.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/grating.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratingfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratinggraph.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulator.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/gratingwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/logger.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/mainpage.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/mask.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/maskfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/maskwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/modifiable.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/mutex.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/namedid.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/oscillator.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/point.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/pointlistwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/pointwidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/positiongen.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/positiongenfwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/ridge.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/ridgefwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/ridgegraph.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/ridgewidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/state.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/tactileengine.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/tactileenginefwd.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/timer.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/units.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/vector.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/velocityestimator.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/virtualsurfacewidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/visualizewidget.h"
    "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/xml.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/liblaterographics.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/vlevesque/Local/Git/OpenLatero/latero-graphics/latero-graphics/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
