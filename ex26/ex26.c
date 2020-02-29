#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include "dbg.h"

#define MAX_DATA 1024

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

        if ((search_type == AND && match_count == wordsc) || (search_type == OR && match_count > 0))
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

int scan_files(glob_t *globbuf)
{
    // open search paths file
    FILE *filePointer = fopen(".logfind", "r");
    check(filePointer, "Cannot open file %s", ".logfind");
    char buffer[MAX_DATA];

    for (size_t i = 0; fgets(buffer, MAX_DATA - 1, filePointer); i++)
    {
        buffer[strcspn(buffer, "\n")] = 0;
        if (i == 0)
            glob(buffer, GLOB_ERR, NULL, globbuf);
        else
            glob(buffer, GLOB_ERR | GLOB_APPEND, NULL, globbuf);
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
    check(argc > 1, "USAGE: ex26 [-o] word1 word2 ...");

    glob_t globbuf;
    scan_files(&globbuf);

    SearchType search_type = strcmp(argv[1], "-o") == 0 ? OR : AND;
    int first_word_index = search_type == OR ? 2 : 1;

    for (int i = 0; i < globbuf.gl_pathc; i++)
    {
        find_match_in_file(globbuf.gl_pathv[i],
                           argc - first_word_index,
                           &argv[first_word_index],
                           search_type);
    }

    globfree(&globbuf);
    return 0;
error:
    globfree(&globbuf);
    return -1;
}