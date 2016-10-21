prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include
	
Name: liblaterographics
Description: rendering code for Latero tactile displays (http://tactilelabs.com/products/haptics/latero-tactile-display/)
Version: 0.0.1

Libs: -L${libdir} -llaterographics @DEPS_LDFLAGS_WSPACE@
Cflags: -I${includedir} @DEPS_CFLAGS_WSPACE@ 