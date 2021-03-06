#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include "dbg.h"
#include <assert.h>

const size_t MAX_DATA = 1024;

typedef enum SearchType
{
    AND,
    OR
} SearchType;

int match_in_file(const char *file_name, size_t max_buffer, size_t wordsc, char *words[], SearchType search_type)
{
    assert(max_buffer >= MAX_DATA && "max_buffer must be at least 1024 for aceptable line length");
    check(file_name != NULL, "file_name cannot be NULL");
    
    FILE *filePointer = fopen(file_name, "r");
    check(filePointer, "Cannot open file %s", file_name);

    char *buffer = calloc(max_buffer, 1);
    check_mem(buffer);

    for (size_t i = 1; fgets(buffer, max_buffer - 1, filePointer); i++)
    {

        size_t match_count = 0;
        for (size_t j = 0; j < wordsc; j++)
        {
            if (strstr(buffer, words[j]))
                match_count++;
        }

        if ((search_type == AND && match_count == wordsc) || (search_type == OR && match_count > 0))
            printf("%s in line %ld: %s", file_name, i, buffer);
    }

    fclose(filePointer);
    free(buffer);
    buffer = NULL;
    return 0;

error:
    if (filePointer)
        fclose(filePointer);
    if (buffer)
        free(buffer);
    return -1;
}

int scan_files(glob_t *globbuf, const char *file_name, size_t max_buffer)
{
    assert(max_buffer >= 50 && "max_buffer must be at least 50 for aceptable file name length");

    check(file_name != NULL, "file_name cannot be NULL");
    check(globbuf != NULL, "Invalid glob_t given.");

    char *buffer = calloc(max_buffer, 1);
    check_mem(buffer);

    // open search paths file
    FILE *filePointer = fopen(file_name, "r");
    check(filePointer, "Cannot open file %s", ".logfind");

    size_t glob_flags = GLOB_ERR;
    int rc = 0;

    for (size_t i = 0; fgets(buffer, max_buffer - 1, filePointer); i++)
    {
        buffer[strcspn(buffer, "\n")] = 0;

        rc = glob(buffer, glob_flags, NULL, globbuf);
        check(rc == 0 || rc == GLOB_NOMATCH, "Failed to glob");
        if (i == 0)
            glob_flags |= GLOB_APPEND;
    }

    fclose(filePointer);
    free(buffer);
    buffer = NULL;
    return rc;
error:
    if (filePointer)
        fclose(filePointer);
    if (buffer)
        free(buffer);
    return rc;
}

int main(int argc, char *argv[])
{
    check(argc > 1, "USAGE: ex26 [-o] word1 word2 ...");

    glob_t globbuf;
    char *list_file_name = ".logfind";
    int rc = 0;
    rc = scan_files(&globbuf, list_file_name, MAX_DATA);
    check(rc == 0, "Failed to open scan file list.");

    SearchType search_type = strcmp(argv[1], "-o") == 0 ? OR : AND;
    int first_word_index = search_type == OR ? 2 : 1;

    for (int i = 0; i < globbuf.gl_pathc; i++)
    {
        rc = match_in_file(globbuf.gl_pathv[i],
                           MAX_DATA,
                           argc - first_word_index,
                           &argv[first_word_index],
                           search_type);
        check(rc == 0, "Failed to find words in filelist.");
    }

    globfree(&globbuf);
    return 0;
error:
    // if (globbuf.gl_pathv)
    //     globfree(&globbuf);
    return 1;
}