///
/// File: fileRead.c
///
/// Description: Used to read in a maze for mopsolver to solve. Uses two methods
///              determined by the method of input (stdin vs. separate file).
///
/// @author kjb2503 : Kevin Becker
///
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _GNU_SOURCE
#include <stdio.h> // printing
#include <string.h> // string functions
#include <stdlib.h> // allocation functions
#include "fileRead.h" // the function we need to write is in here


///
/// Function: fileStringCat
///
/// Description: Used to concatenate the existing string with a newly read in line.
///
/// @param ** fileString  The existing fileString to be added to.
/// @param *str  The string to add to fileString.
///
static void fileStringCat(char **fileString, char *str)
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
    *fileString = realloc(*fileString, (strlen(*fileString) + strlen(str) + 1));

    //concatenates the strings together
    strncat(*fileString, str, length);
}

///
/// Function: readFromStdin
///
/// Description: The function used to read in a string from stdin.
///
/// @return A string read in from the stdin buffer.
///

static char * readFromStdin()
{
    // the line buffer getline will write to
    char *buf = NULL;
    // the line size which getline will change
    size_t bufsize = 0;
    // allocates our empty file string
    char *fileString = malloc(strlen("") +1);
    // copies in an empty string
    strcpy(fileString, "");

    // READING IN FILE PROCEDURE ===============================================

    // while we still have lines to read
    while(getline(&buf, &bufsize, stdin) > 0)
        fileStringCat(&fileString, buf);

    // frees our buffer; we're done with it
    free(buf);
    buf = NULL;

    // return our file
    return fileString;
}

///
/// Function: readFromDisk
///
/// Description: Used to read a file that exists on the disk.
///
/// @param fileIn  The file to read from.
///
static char * readFromDisk(FILE * fileIn)
{
    // seeks the end of the file
    fseek(fileIn, 0, SEEK_END);
    // tells us how long to make our buffer
    size_t fileSize = (size_t) ftell(fileIn);
    // jumps back to the beginning of file
    fseek(fileIn, 0, SEEK_SET);

    // allocates enough space into our buffer
    char *fileString = malloc(fileSize + 1);
    // reads in the file and drops it in file
    fread(fileString, fileSize, 1, fileIn);

    // returns our newly constructed char array
    return fileString;
}

/// reads in a file and returns it as a string
char * getFileAsString(FILE * fileIn)
{
    // returns the file as a character array (string)
    return (fileIn == stdin) ? readFromStdin() : readFromDisk(fileIn);
}
