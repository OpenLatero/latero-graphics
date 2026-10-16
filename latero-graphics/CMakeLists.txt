# set file lists and install rules for subdirectories
add_subdirectory(gtk)
add_subdirectory(graphics)

# local sources
set(SRC_ROOT_CPP
	audiodevice.cpp
	audioengine.cpp
	averagefilter.cpp
	engine.cpp
	fileutils.cpp
	generator.cpp
	graph.cpp
	grating.cpp gratingwidget.cpp
	gratinggraph.cpp
	gratingmodulator.cpp gratingmodulatorwidget.cpp
	logger.cpp
	mask.cpp maskwidget.cpp
	mutex.cpp
	oscillator.cpp
	oscillatorwidget.cpp
	pointlistwidget.cpp
	pointwidget.cpp
	positiongen.cpp
	ridge.cpp ridgewidget.cpp
	ridgegraph.cpp
	tactileengine.cpp
	tactileenginestats.cpp
	vector.cpp
	velocityestimator.cpp
	tactiledisplaypainter.cpp
	tactiledisplayview.cpp
	tactographview.cpp
	visualizewidget.cpp
	xml.cpp
)

set(SRC_ROOT_H
	audiodevice.h
	audioengine.h
	averagefilter.h
	buttonevent.h
	engine.h
	external.h
	fileutils.h
	generator.h
	graph.h
	grating.h
	gratinggraph.h
	gratingmodulator.h
	gratingmodulatorwidget.h
	gratingwidget.h
	logger.h
	mainpage.h
	mask.h
	maskwidget.h
	modifiable.h
	mutex.h
	namedid.h
	oscillator.h
	oscillatorwidget.h
	pointlistwidget.h
	pointwidget.h
	positiongen.h
	ridge.h
	ridgegraph.h
	ridgewidget.h
	state.h
	tactileengine.h
	tactileenginestats.h
	timer.h
	units.h
	vector.h
	velocityestimator.h
	tactiledisplaypainter.h
	tactiledisplayview.h
	tactographview.h
	visualizewidget.h
	xml.h
)
set(SRC_ROOT ${SRC_ROOT_H} ${SRC_ROOT_CPP})

# install header files to /usr/local/include/laterographics
INSTALL(FILES ${SRC_ROOT_H} DESTINATION include/laterographics)

# combines all sources
set(SRC ${SRC_ROOT_H} ${SRC_ROOT_CPP} ${SRC_GTK} ${SRC_GRAPHICS})

SOURCE_GROUP(main FILES ${SRC_ROOT})
SOURCE_GROUP(gtk FILES ${SRC_GTK})
SOURCE_GROUP(graphics FILES ${SRC_GRAPHICS})

# build
add_library (laterographics ${SRC})
add_library(laterographics::laterographics ALIAS laterographics)
target_link_libraries(laterographics PUBLIC 
	latero::latero
	ImageMagick::Magick++
	PkgConfig::GTKMM
	PkgConfig::LIBXML)

# location of the installed stock textures (see graphics/texture/stock.h)
target_compile_definitions(laterographics PUBLIC
	"LATERO_GRAPHICS_TX_PATH=\"${CMAKE_INSTALL_PREFIX}/${LATERO_GRAPHICS_TX_DIR}/\"")

if (WIN32)
	# timing of the real-time update loop (private, not installed)
	target_sources(laterographics PRIVATE wintiming.cpp wintiming.h)
	target_link_libraries(laterographics PRIVATE avrt winmm)
endif()

#install library files to /usr/local/lib/
INSTALL(TARGETS laterographics
 EXPORT laterographicsTargets
 RUNTIME DESTINATION bin
 LIBRARY DESTINATION lib
 ARCHIVE DESTINATION lib
 INCLUDES DESTINATION include
)

install(EXPORT laterographicsTargets
    FILE laterographicsTargets.cmake
    NAMESPACE laterographics::
    DESTINATION lib/cmake/laterographics
)


# Treat deprecation in GTKMM as errors
#target_compile_definitions(laterographics PRIVATE GLIB_DISABLE_DEPRECATED GDK_DISABLE_DEPRECATED GTK_DISABLE_DEPRECATED GTKMM_DISABLE_DEPRECATED)