#pragma once

/**
 * Windows compatibility definitions. On Windows, CMake force-includes this header in every source
 * file of the library and of the programs that use it (see latero-graphics/CMakeLists.txt).
 */
#ifdef _WIN32

#include <io.h>
#include <wchar.h>

/* provided by <sys/types.h> on Linux and macOS, but not on Windows */
typedef unsigned int uint;

/* std::filesystem::path::c_str() returns a wide string on Windows */
inline int chmod(const wchar_t *path, int mode) { return _wchmod(path, mode); }

#endif
