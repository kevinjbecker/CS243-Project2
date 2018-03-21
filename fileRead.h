///
/// File: fileRead.c
///
/// Description: Used to read in a maze for mopsolver to solve.
///
/// @author kjb2503 : Kevin Becker
///
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#ifndef _FILE_READ
#define _FILE_READ // include guard

///
/// Function: getFileAsString
///
/// Description: Reads in a file from the specified file and returns it as a string.
///
/// @param fileIn  The file to read from.
///
/// @return the file read in as a single string.
///
char * getFileAsString(FILE * fileIn);


#endif
