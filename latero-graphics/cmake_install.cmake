# Install script for directory: /Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics

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

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/laterographics" TYPE FILE FILES
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/audiodevice.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/audiodevicefwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/audioengine.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/audioenginefwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/averagefilter.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/buttonevent.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/engine.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/external.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/generator.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/generatorfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/graph.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/grating.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratingfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratinggraph.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratingmodulator.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratingmodulatorwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gratingwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/logger.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/mainpage.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/mask.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/maskfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/maskwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/modifiable.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/mutex.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/namedid.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/oscillator.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/oscillatorfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/oscillatorwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/point.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/pointlistwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/pointwidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/positiongen.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/positiongenfwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/ridge.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/ridgefwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/ridgegraph.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/ridgewidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/state.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/tactileengine.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/tactileenginefwd.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/timer.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/units.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/vector.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/velocityestimator.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/virtualsurfacewidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/visualizewidget.h"
    "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/xml.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/Debug/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/Release/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/MinSizeRel/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/RelWithDebInfo/liblaterographics.a")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblaterographics.a")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/gtk/cmake_install.cmake")
  include("/Users/tactilelabs/Documents/OpenLatero/latero-graphics/latero-graphics/graphics/cmake_install.cmake")

endif()

