#include <stdio.h>
#include <dlfcn.h>
#include "dbg.h"

typedef int (*lib_function)(const char *data);

int call_dynamic_lib(const char *lib_file, const char *func_to_run, const char *data)
{

    int rc = 0;

    void *lib = dlopen(lib_file, RTLD_NOW);
    check(lib != NULL, "Failed to open the library %s: %s", lib_file, dlerror());

    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL,
          "Did not find %s function in the library %s: %s",
          func_to_run, lib_file, dlerror());

    rc = func(data);
    check(rc == 0, "Function %s return %d for data %s", func_to_run, rc, data);

    rc = dlclose(lib);
    check(rc == 0, "Failed to close %s", lib_file);

    return 0;
error:
    return -1;
}

int main(void)
{

    int rc = 0;
    // check(argc == 4, "USAGE: ex29 libex28.so function data");

    const char *lib_file = "./build/libex29.so";
    char *func_to_run = "uppercase";
    char *data = "hola mundo";

    rc = call_dynamic_lib(lib_file, func_to_run, data);
    rc = call_dynamic_lib(lib_file, "lowercase", "HOLA MUNDO");
    rc = call_dynamic_lib(lib_file, "print_a_message", "HEY THERREEE!!!");

    return rc;
}
