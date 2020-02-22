#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifndef __FUNCTION_NAME__
#ifdef WIN32 //WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "%s - %s: [DEBUG] %s:%d:%s " M " \n", \
                              __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "%s - %s: [ERROR] (%s:%d:%s: errno: %s) " M "\n", \
                                __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION_NAME__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "%s - %s: [WARN] (%s:%d:%s errno: %s) " M "\n", \
                                 __DATE__, __TIME__, __FILE__, __LINE__, __FUNCTION_NAME__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "%s - %s: [INFO] (%s:%d:%s) " M "\n", __DATE__, \
                                 __TIME__, __FILE__, __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)

#define check(A, M, ...)           \
    if (!(A))                      \
    {                              \
        log_err(M, ##__VA_ARGS__); \
        errno = 0;                 \
        goto error;                \
    }
#define sentinel(M, ...)           \
    {                              \
        log_err(M, ##__VA_ARGS__); \
        errno = 0;                 \
        goto error;                \
    }
#define check_mem(A) check(A, "Out of memory.")

#define check_debug(A, M, ...)   \
    if (!(A))                    \
    {                            \
        debug(M, ##__VA_ARGS__); \
        errno = 0;               \
        goto error;              \
    }

#endif