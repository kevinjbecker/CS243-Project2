///
/// File: mopsolver.c
///
/// Description: Takes a maze "construction" file as input and attempts to find
///              the shortest distance from start to finish.
///
/// @author kjb2503 : Kevin Becker
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _DEFAULT_SOURCE
#include <unistd.h> // getopt
#include <stdio.h> // printing
#include <string.h>
#include <stdlib.h>

#define WALL 'O'
#define EMPTY ' '

// our Queue node system
typedef struct qnode_s{
    struct qnode_s *next;
    size_t x, y; // the x and y coordinates
} QNode;


void printHelpMsg(char *start)
{
    // prints usage and exits
    printf("Usage:\n"
           "%s [-hbsmp] [-i INFILE] [-o OUTFILE]\n\n"
           "Options:\n"
           "-h Prints this message to stdout and exits.\n"
           "-b Add borders and pretty-print.     (Default: off)\n"
           "-s Add shortest solution step total. (Default: off)\n"
           "-m Print matrix after reading.       (Default: off)\n"
           "-p Print solution with path.         (Default: off)\n"
           "-i INFILE Read maze from INFILE      (Default: stdin)\n"
           "-o OUTFILE Write maze to OUTFILE     (Default: stdout)\n", start);
}


static void fileCat(char **dest, const char *str)
{
    // resizes the "file" string
    *dest = realloc(*dest, (strlen(*dest) + strlen(str) + 1));
    
    //concatenates the strings
    strcat(*dest, str);
}


static char **processFile(char *file)
{
    // the data we need to store (on the stack)
    size_t rows, i = 0, cols = 0;
    // we count the number of columns to include
    while(file[i] != '\n')
        if(file[i++] != ' ')
            ++cols;
    
    /* the number of columns is equal to the size of the string divided by two 
       times the number of columns */
    rows = (strlen(file) / (2 * cols));
    
    // begins to build our maze by first allocating our space
    char **maze = NULL;
    // contiguously allocates the right number of rows
    maze = calloc(sizeof(char *), rows);
    
    // contiguously allocates the columns for each row
    for(size_t row = 0; row < rows; ++row)
        maze[row] = calloc(sizeof(char), cols);
    
    return NULL;
}


int main(int argc, char **argv)
{
    // these are used for after we read in our stuff
    unsigned char prettyPrint = 0, solutionSteps = 0, matrix = 0, path = 0;
    FILE *fileIn = stdin, *fileOut = stdout;
    char opt;
    // parses our flags (if any are present)
    while((opt = getopt(argc, argv, "hbsmpi:o:")) != -1)
    {
        switch(opt)
        {
            // help menu and quit
            case 'h':
                printHelpMsg(argv[0]);
                return EXIT_SUCCESS;
                break;
            // flad which will add border and pretty print the read in maze
            case 'b': 
                prettyPrint = 1;
                break;
            // flag to print the number of steps in the solution
            case 's':
                solutionSteps = 1;
                break;
            // flag which will print out our read in matrix
            case 'm':
                matrix = 1;
                break;
            // flad which will print our solution with path
            case 'p':
                path = 1;
                break;
            // flag preset to set our fileIn
            case 'i':
                // opens the in file in read-only mode
                fileIn = fopen(optarg, "r");
                // if the file doesn't exist (i.e. fopen returns NULL), print err.
                if(fileIn == NULL)
                    perror("Error opening input file");
                break;
            case 'o':
                // open our output file in a+ mode which will create the file
                // if it doesn't exist
                fileOut = fopen(optarg, "a+");
                // if we have an error we report it here
                if(fileOut == NULL)
                    perror("Error opening output file");
                break;
        }
    }
    
    printf("status: pret=%d, sol=%d, mat=%d, pat=%d\n", prettyPrint, solutionSteps, matrix, path);
    
    char *line = NULL;
    size_t linesize = 0;
    // allocates our empty filestring
    char *file = malloc(strlen("") +1);
    strcpy(file, "");
    
    while(getline(&line, &linesize, fileIn) > 0)
        fileCat(&file, line);
    
    // frees our line; we're done with it
    free(line);
    line = NULL;
    
    
    // prints our matrix if we were asked to do so by the user
    // there is a new line at the end of our matrix so we don't need to add that.
    if(matrix)
        printf("Read this matrix:\n%s", file);
    
    // process file string to create our maze
    char **maze = processFile(file);
    
    // frees our file; we're done with it
    free(file);
    file = NULL;

    return EXIT_SUCCESS;
}

