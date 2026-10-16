#pragma once

#include <filesystem>

namespace latero::graphics {

/**
 * Makes a file readable and writable by all users (a directory also accessible), so that the files saved
 * by a program running as root (e.g. for real-time priority) can be used by others. Failures are ignored.
 */
void MakeAccessibleToAll(const std::filesystem::path& path);

} // namespace
