#ifndef FMI4CPP_LIBRARYHELPER_HPP
#define FMI4CPP_LIBRARYHELPER_HPP

#include <fmi4cpp/dll_handle.hpp>
#include <sstream>

namespace fmi4cpp
{

inline DLL_HANDLE load_library(const std::string& libName)
{
#ifdef WIN32
    std::wstring libNameTemp = std::wstring(libName.begin(), libName.end());
    return LoadLibrary(libNameTemp.c_str());
#else
    return dlopen(libName.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
}

template<class T>
T load_function(DLL_HANDLE handle, const char* function_name)
{
#ifdef WIN32
    return (T)GetProcAddress(handle, function_name);
#else
    return (T)dlsym(handle, function_name);
#endif
}

inline bool free_library(DLL_HANDLE handle)
{
#ifdef WIN32
    return static_cast<bool>(FreeLibrary(handle));
#else
    return (dlclose(handle) == 0);
#endif
}

inline std::string getLastError()
{
#ifdef WIN32
    std::ostringstream os;

    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return "";
    }

    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, nullptr);

    os << errorMessageID << " " << messageBuffer;
    LocalFree(messageBuffer);

    return os.str();
#else
    return dlerror();
#endif
}

} // namespace

#endif //FMI4CPP_LIBRARYHELPER_HPP
