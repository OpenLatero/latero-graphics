set(SRC_CPP 
	animation.cpp
	checkframe.cpp
	imagedropdown.cpp
	numwidget.cpp
	pixbufops.cpp
	plot.cpp
)

set(SRC_H 
	animation.h
	checkframe.h
	imagedropdown.h
	numwidget.h
	pixbufops.h
	plot.h
)

set(SRC ${SRC_H} ${SRC_CPP})
SOURCE_GROUP(gtk FILES ${SRC})

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/gtk)

SET(SRC_GTK "" CACHE INTERNAL "")
FOREACH(file ${SRC})
     SET(SRC_GTK ${SRC_GTK} gtk/${file} CACHE INTERNAL "")
ENDFOREACH(file)
