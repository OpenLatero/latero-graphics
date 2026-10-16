#include "fileutils.h"

namespace latero::graphics {

void MakeAccessibleToAll(const std::filesystem::path& path)
{
	namespace fs = std::filesystem;
	std::error_code ec;
	fs::perms perms = fs::is_directory(path, ec) ? fs::perms::all : // 0777
		fs::perms::owner_read | fs::perms::owner_write |
		fs::perms::group_read | fs::perms::group_write |
		fs::perms::others_read | fs::perms::others_write; // 0666
	fs::permissions(path, perms, ec);
}

} // namespace
