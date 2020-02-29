#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include "dbg.h"

#define MAX_DATA 1024
#define MAX_FILES 100

typedef enum SearchType
{
    AND,
    OR
} SearchType;

int find_match_in_file(char *file_name, int wordsc, char **words, SearchType search_type)
{
    FILE *filePointer = fopen(file_name, "r");
    check(filePointer, "Cannot open file %s", file_name);
    char buffer[MAX_DATA];
    int match_count = 0;

    for (size_t i = 1; fgets(buffer, MAX_DATA - 1, filePointer); i++)
    {
        for (size_t j = 0; j < wordsc; j++)
        {
            if (strstr(buffer, words[j]))
                match_count++;
        }

        if (search_type == AND && match_count == wordsc)
            printf("%s in line %ld: %s", file_name, i, buffer);
        else if (search_type == OR && match_count > 0)
            printf("%s in line %ld: %s", file_name, i, buffer);

        match_count = 0;
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
    FILE *filePointer = fopen(".logfind", "r");
    check(filePointer, "Cannot open file %s", ".logfind");
    char buffer[MAX_DATA];
    glob_t globbuf;

    for (size_t i = 0; fgets(buffer, MAX_DATA - 1, filePointer); i++)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        if (i == 0)
            glob(buffer, GLOB_DOOFFS, NULL, &globbuf);
        else
            glob(buffer, GLOB_DOOFFS | GLOB_APPEND, NULL, &globbuf);
    }

    check(argc > 1, "USAGE: ex26 [-o] word1 word2 ...");

    for (int i = 1; i <= globbuf.gl_pathc; i++)
    {
        if (strcmp(argv[1], "-o") == 0)
        {
            find_match_in_file(globbuf.gl_pathv[i], argc - 2, &argv[2], OR);
        }
        else
        {
            find_match_in_file(globbuf.gl_pathv[i], argc - 1, &argv[1], AND);
        }
    }
    globfree(&globbuf);
    fclose(filePointer);
    return 0;
error:
    if (filePointer)
        fclose(filePointer);
    globfree(&globbuf);
    return -1;
}