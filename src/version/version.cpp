#include "version.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

std::string_view get_version()
{
    return TOSTRING(GIT_VERSION);
}
