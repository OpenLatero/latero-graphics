# Install script for directory: /Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/laterographics" TYPE FILE FILES
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audiodevice.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audiodevicefwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audioengine.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/audioenginefwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/averagefilter.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/buttonevent.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/engine.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/external.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/generator.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/generatorfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/graph.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/grating.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratinggraph.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulator.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gratingwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/logger.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mainpage.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mask.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/maskfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/maskwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/modifiable.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/mutex.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/namedid.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillator.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/oscillatorwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/point.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/pointlistwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/pointwidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/positiongen.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/positiongenfwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridge.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgefwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgegraph.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/ridgewidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/state.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/tactileengine.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/tactileenginefwd.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/timer.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/units.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/vector.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/velocityestimator.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/virtualsurfacewidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/visualizewidget.h"
    "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/xml.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/Debug/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/Release/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/MinSizeRel/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/RelWithDebInfo/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/gtk/cmake_install.cmake")
  include("/Users/vlevesque/Documents/Git/OpenLatero/latero-graphics/latero-graphics/graphics/cmake_install.cmake")

endif()

