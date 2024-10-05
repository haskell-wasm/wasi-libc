#include <wasi/api.h>
#include <__errno.h>
#include <__function___isatty.h>

int __isatty(int fd) {
    __wasi_fdstat_t statbuf;
    int r = __wasi_fd_fdstat_get(fd, &statbuf);
    if (r != 0) {
        errno = r;
        return 0;
    }

    // A tty is a character device that we can't seek or tell on.
    if (statbuf.fs_filetype != __WASI_FILETYPE_CHARACTER_DEVICE ||
        (statbuf.fs_rights_base & (__WASI_RIGHTS_FD_SEEK | __WASI_RIGHTS_FD_TELL)) != 0) {
        errno = __WASI_ERRNO_NOTTY;
        return 0;
    }

    return 1;
}

// The public-facing isatty should always return ENOTTY. Otherwise
// false positives cause much suffering.
int __notatty(int fd);
int __notatty(int fd) {
    errno = __WASI_ERRNO_NOTTY;
    return 0;
}

extern __typeof(__notatty) isatty __attribute__((weak, alias("__notatty")));
