set(SRC_CPP
	dirmodulator.cpp
	dirmodulatorwidget.cpp
	modulatordropdown.cpp
	modulator.cpp
	modulatorpreview.cpp
	velmodulator.cpp
	velmodulatorwidget.cpp
)

set(SRC_H
	dirmodulator.h
	dirmodulatorwidget.h
	modulatordropdown.h
	modulator.h
	modulatorpreview.h
	velmodulator.h
	velmodulatorwidget.h
)

set(SRC ${SRC_H} ${SRC_CPP})
SOURCE_GROUP(modulator FILES ${SRC})

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/graphics/modulator)

SET(SRC_GRAPHICS_MOD "" CACHE INTERNAL "")
FOREACH(file ${SRC})
     SET(SRC_GRAPHICS_MOD "${SRC_GRAPHICS_MOD};modulator/${file}" CACHE INTERNAL "")
ENDFOREACH(file)