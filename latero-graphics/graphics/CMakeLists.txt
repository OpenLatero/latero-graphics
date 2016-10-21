add_subdirectory(modulator)
add_subdirectory(texture)

set(SRC_CPP
audiopattern.cpp
canvas.cpp
canvaswidget.cpp
circle.cpp
circlewidget.cpp
closedpattern.cpp
closedpatternwidget.cpp
dot.cpp
dotpattern.cpp
dotpatternwidget.cpp
dots.cpp
dotswidget.cpp
dotwidget.cpp 
group.cpp
groupwidget.cpp
image.cpp
imagewidget.cpp
line.cpp
linewidget.cpp
pattern.cpp
patterncreatordialog.cpp
patternpreview.cpp
polygon.cpp
polygonwidget.cpp
stroke.cpp
strokemotionwidget.cpp
strokewidget.cpp
toggle.cpp
togglewidget.cpp
)

set(SRC_H
audiopattern.h 
audiopatternfwd.h 
canvas.h 
canvasfwd.h 
canvaswidget.h 
circle.h 
circlefwd.h 
circlewidget.h 
closedpattern.h 
closedpatternfwd.h 
closedpatternwidget.h 
dot.h
dotfwd.h
dotpattern.h 
dotpatternfwd.h 
dotpatternwidget.h 
dots.h 
dotsfwd.h
dotswidget.h 
dotwidget.h 
group.h
groupfwd.h
groupwidget.h 
image.h
imagefwd.h
imagewidget.h 
line.h 
linefwd.h 
linewidget.h 
pattern.h
patterncreatordialog.h 
patternfwd.h 
patternpreview.h 
polygon.h
polygonfwd.h 
polygonwidget.h 
stroke.h 
strokefwd.h 
strokemotionwidget.h
strokewidget.h
tactileops.h
toggle.h
togglefwd.h
togglewidget.h
)

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/graphics)

set(SRC ${SRC_H} ${SRC_CPP} ${SRC_GRAPHICS_MOD} ${SRC_GRAPHICS_TX})
SOURCE_GROUP(graphics FILES ${SRC})

SET(SRC_GRAPHICS "")
FOREACH(file ${SRC})
     SET(SRC_GRAPHICS "${SRC_GRAPHICS};graphics/${file}" CACHE INTERNAL "")
ENDFOREACH(file)
