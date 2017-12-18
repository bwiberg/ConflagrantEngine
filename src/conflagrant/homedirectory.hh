#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstdlib>

namespace cfl {
inline auto GetHomeDirectory() {
    static const char *homedir;

    if (homedir == nullptr) {
        if ((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }
    }

    return homedir;
}
} // namespace cfl
