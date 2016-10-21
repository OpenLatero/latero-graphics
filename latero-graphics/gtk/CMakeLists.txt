set(SRC_CPP 
	animation.cpp
	blobdetector.cpp
	checkframe.cpp
	imagecombo.cpp
	numwidget.cpp
	pixbufops.cpp
	plot.cpp
)

set(SRC_H 
	animation.h
	blobdetector.h
	checkframe.h
	imagecombo.h
	numwidget.h
	pixbufops.h
	plot.h
	spacer.h
)

set(SRC ${SRC_H} ${SRC_CPP})
SOURCE_GROUP(gtk FILES ${SRC})

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/gtk)

SET(SRC_GTK "")
FOREACH(file ${SRC})
     SET(SRC_GTK ${SRC_GTK} gtk/${file}  CACHE INTERNAL "")
ENDFOREACH(file)
