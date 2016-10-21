set(SRC_H
	texture.h
 	texturefwd.h
	texturewidget.h
	gratingtexture.h
	gratingtexturefwd.h
	gratingtexturewidget.h
	axialgratingtexturefwd.h
	combotexturefwd.h
	motiontexture.h
	patterntexture.h
	radialgratingtexture.h
	textureselectorwidget.h
	axialgratingtexture.h
	combotexture.h
	motiontexturewidget.h
	patterntexturewidget.h
	radialgratingtexturewidget.h
	axialgratingtexturewidget.h
	doublelineargratingtexturefwd.h
	lineargratingtexturefwd.h
	nulltexturefwd.h
	plaintexturefwd.h
	stock.h
	vibrotexturefwd.h
	blendtexturefwd.h
	doublelineargratingtexture.h
	lineargratingtexture.h
	nulltexture.h
	plaintexture.h
	texturecombo.h
	vibrotexture.h
	blendtexture.h
	doublelineargratingtexturewidget.h
	lineargratingtexturewidget.h
	patternops.h
	plaintexturewidget.h
	vibrotexturewidget.h
	blendtexturewidget.h
	motiontexturefwd.h
	patterntexturefwd.h
	radialgratingtexturefwd.h
)

set(SRC_CPP
	texture.cpp
	texturewidget.cpp
	gratingtexture.cpp
	gratingtexturewidget.cpp
	axialgratingtexture.cpp
	doublelineargratingtexture.cpp
	lineargratingtexturewidget.cpp
	patterntexture.cpp
	radialgratingtexturewidget.cpp
	vibrotexture.cpp
	axialgratingtexturewidget.cpp
	doublelineargratingtexturewidget.cpp
	motiontexture.cpp
	patterntexturewidget.cpp
	texturecombo.cpp
	vibrotexturewidget.cpp
	blendtexture.cpp
	motiontexturewidget.cpp
	plaintexture.cpp
	blendtexturewidget.cpp
	nulltexture.cpp
	plaintexturewidget.cpp
	textureselectorwidget.cpp
	combotexture.cpp
	lineargratingtexture.cpp
	patternops.cpp
	radialgratingtexture.cpp
)

set(SRC ${SRC_H} ${SRC_CPP})
SOURCE_GROUP(texture FILES ${SRC})

INSTALL(FILES ${SRC_H} DESTINATION include/laterographics/graphics/texture)

SET(SRC_GRAPHICS_TX "")
FOREACH(file ${SRC})
     SET(SRC_GRAPHICS_TX "${SRC_GRAPHICS_TX};texture/${file}" CACHE INTERNAL "")
ENDFOREACH(file)
