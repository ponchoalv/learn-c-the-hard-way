#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include "dbg.h"

#define MAX_DATA 1024
#define MAX_FILES 100

int read_string(int max_buffer, char *file_name)
{
    FILE *filePointer = fopen(file_name, "r");
    check(filePointer, "Cannot open file %s", file_name);
    char buffer[MAX_DATA];

    while(fgets(buffer, max_buffer, filePointer)){
        printf("%s", buffer);
    }

    fclose(filePointer);
    return 0;

error:
     if (filePointer)
        fclose(filePointer);
    return -1;
}

int main(int argc, char *argv[])
{
    glob_t globbuf;

    glob("*.c", GLOB_DOOFFS, NULL, &globbuf);
    glob("../*.c", GLOB_DOOFFS | GLOB_APPEND, NULL, &globbuf);

    globfree(&globbuf);

    read_string(MAX_DATA, ".logfind");
    return 0;
}