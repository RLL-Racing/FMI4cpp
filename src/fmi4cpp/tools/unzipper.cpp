
#include "fmi4cpp/tools/unzipper.hpp"

#include <Utilities/Messenger.hpp>

#include <fstream>
#include <string>


bool fmi4cpp::unzip(const std::filesystem::path& zip_file, const std::filesystem::path& tmp_path)
{
    // Check if the zip file exists
    if (!exists(zip_file)) {
        LOG_CORE_ERROR("Zip file does not exist: {0}", std::filesystem::absolute(zip_file).string().c_str());
        return false;
    }

#ifdef _WIN32
    // Windows: Use the built-in tar command
    std::string command = "tar -xf \"" + zip_file.string() + "\" -C \"" + tmp_path.string() + "\"";
#else
    // Linux: Use the unzip command
    std::string command = "unzip -o \"" + zip_file.string() + "\" -d \"" + tmp_path.string() + "\" > /dev/null 2>&1";
#endif

    // Execute the command
    const int result = std::system(command.c_str());

    if (result != 0) {
        LOG_CORE_ERROR("Unzipping failed with code: {0}", result);
        return false;
    }

    return true;
}
