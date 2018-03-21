///
/// File: 
///
/// Description: 
///
/// @author kjb2503 : Kevin Becker
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _DEFAULT_SOURCE
#include <stdio.h> // printing
#include <string.h> // string functions
#include <stdlib.h> // allocation functions
#include "fileRead.h"

static void fileStringCat(char **file, char *str)
{
    // gets the length of the string
    size_t length = strlen(str);
    // if it is odd, we might have a trailing space (need to deal with that)
    if(length % 2 == 1)
    {
        str[length-2] = '\n';
        --length;
    }

    // resizes the "file" string
    *file = realloc(*file, (strlen(*file) + strlen(str) + 1));
    
    //concatenates the strings together
    strncat(*file, str, length);
}


char * getFileAsString(FILE *fileIn)
{
    // the line buffer getline will write to
    char *buf = NULL;
    // the line size which getline will change
    size_t bufsize = 0;
    // allocates our empty file string
    char *file = malloc(strlen("") +1);
    // copies in an empty string
    strcpy(file, "");
 
    // READING IN FILE PROCEDURE ===============================================
       
    // while we still have lines to read
    while(getline(&buf, &bufsize, fileIn) > 0)
        fileStringCat(&file, buf);
    
    // frees our buffer; we're done with it
    free(buf);
    buf = NULL;
    
    // return our file
    return file;
}
