set(SRC_CPP
	dirmodulator.cpp
	dirmodulatorwidget.cpp
	modulatorcombo.cpp
	modulator.cpp
	modulatorpreview.cpp
	velmodulator.cpp
	velmodulatorwidget.cpp
)

set(SRC_H
	dirmodulatorfwd.h
	dirmodulator.h
	dirmodulatorwidget.h
	modulatorcombo.h
	modulatorfwd.h
	modulator.h
	modulatorpreview.h
	velmodulatorfwd.h
	velmodulator.h
	velmodulatorwidget.h
)

set(SRC ${SRC_H} ${SRC_CPP})
SOURCE_GROUP(modulator FILES ${SRC})

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/graphics/modulator)

SET(SRC_GRAPHICS_MOD "")
FOREACH(file ${SRC})
     SET(SRC_GRAPHICS_MOD "${SRC_GRAPHICS_MOD};modulator/${file}" CACHE INTERNAL "")
ENDFOREACH(file)