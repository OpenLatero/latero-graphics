# set file lists and install rules for subdirectories
add_subdirectory(gtk)
add_subdirectory(graphics)

# local sources
set(SRC_ROOT_CPP
	audiodevice.cpp
	audioengine.cpp
	averagefilter.cpp
	engine.cpp
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
	vector.cpp
	velocityestimator.cpp
	virtualsurfacewidget.cpp
	visualizewidget.cpp
	xml.cpp
)

set(SRC_ROOT_H
	audiodevice.h
	audiodevicefwd.h
	audioengine.h
	audioenginefwd.h
	averagefilter.h
	buttonevent.h
	engine.h
	external.h
	generator.h
	generatorfwd.h
	graph.h
	grating.h
	gratingfwd.h
	gratinggraph.h
	gratingmodulator.h
	gratingmodulatorfwd.h
	gratingmodulatorwidget.h
	gratingwidget.h
	logger.h
	mainpage.h
	mask.h
	maskfwd.h
	maskwidget.h
	modifiable.h
	mutex.h
	namedid.h
	oscillator.h
	oscillatorfwd.h
	oscillatorwidget.h
	point.h
	pointlistwidget.h
	pointwidget.h
	positiongen.h
	positiongenfwd.h
	ridge.h
	ridgefwd.h
	ridgegraph.h
	ridgewidget.h
	state.h
	tactileengine.h
	tactileenginefwd.h
	timer.h
	units.h
	vector.h
	velocityestimator.h
	virtualsurfacewidget.h
	visualizewidget.h
	xml.h
)
set(SRC_ROOT ${SRC_ROOT_H} ${SRC_ROOT_CPP})

# install header files to /usr/local/include/laterographics
INSTALL(FILES ${SRC_ROOT_H} DESTINATION include/laterographics)

# combines all sources
set(SRC ${SRC_ROOT_H} ${SRC_ROOT_CPP} ${SRC_GTK} ${SRC_GRAPHICS} ${SRC_XENO})

SOURCE_GROUP(main FILES ${SRC_ROOT})
SOURCE_GROUP(gtk FILES ${SRC_GTK})
SOURCE_GROUP(graphics FILES ${SRC_GRAPHICS})
SOURCE_GROUP(xeno FILES ${SRC_XENO})

# build
add_library (laterographics ${SRC})

#install library files to /usr/local/lib/
INSTALL(TARGETS laterographics
 RUNTIME DESTINATION bin
 LIBRARY DESTINATION lib
 ARCHIVE DESTINATION lib
)
