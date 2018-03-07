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


static void fileCat(char **file, const char *str)
{
    // resizes the "file" string
    *file = realloc(*file, (strlen(*file) + strlen(str) + 1));
    
    //concatenates the strings
    strcat(*file, str);
}


static size_t getNumCols(char *file)
{
    // index counter, cols counter
    size_t i = 0, cols = 0;
    
    // counts the number of non-space characters before first new line
    while(file[i] != '\n')
        if(file[i++] != ' ')
            ++cols;
    
    // returns the number of columns we have
    return cols;
}


static char ** processMaze(const char *file, 
                           const size_t rows, 
                           const size_t cols)
{
    // row/column index counters
    size_t r, c;
    
    // our eventual storage location for our maze
    char **maze = NULL;
    // contiguously allocates the right number of rows
    maze = calloc(sizeof(char *), rows);
    
    // contiguously allocates the columns for each row
    for(r = 0; r < rows; ++r)
        maze[r] = calloc(sizeof(char), cols);
    
    /* goes through each row and column and sets it to the proper value based
       on the file string 
       NOTE: the index in the file string is: (row * rows * 2) + (col * 2) */
    for(r = 0; r < rows; ++r)
        for(c = 0; c < cols; ++c)
            maze[r][c] = (file[(rows * (r * 2)) + (c * 2)] == '0') ? 0 : 1;
    
    // we have finished building our maze we can now return it
    return maze;
}

static void emptyMaze(char **maze, const size_t rows)
{
    // frees each individual row
    for(size_t r = 0; r < rows; ++r)
        free(maze[r]);
    
    // finally frees our maze
    free(maze);
}


static void printBorder(const size_t cols)
{
    // prints our top border
    for(size_t i = 0; i < cols * 2 + 3; ++i)
        printf("%c", WALL);
    // prints the new line character at the end
    printf("\n");
}


static void prettyPrintMaze(char **maze, const size_t rows, const size_t cols)
{
    // prints our border
    printBorder(cols);
    
    // goes through and prints each character
    for(size_t r = 0; r < rows; ++r)
    {
        // if r is anything but 0, print a wall (border)
        printf("%c", (r) ? WALL : EMPTY);
        for(size_t c = 0; c < cols; ++c)
            printf(" %c", (maze[r][c]) ? WALL : EMPTY);
        // if r is anything but rows-1 print a wall (border)
        printf(" %c\n", (r != rows-1) ? WALL : EMPTY);
    }
    
    // again prints our border
    printBorder(cols);
}


static char * readMazeFromFile(FILE *fileIn)
{
    // the line buffer getline will write to
    char *buf = NULL;
    // the linesize getline will change
    size_t bufsize = 0;
    // allocates our empty filestring
    char *file = malloc(strlen("") +1);
    // copies in an empty string
    strcpy(file, "");
    
    // while we still read in lines
    while(getline(&buf, &bufsize, fileIn) > 0)
        fileCat(&file, buf);
    
    // frees our buffer; we're done with it
    free(buf);
    buf = NULL;
    
    // return our file
    return file;
}


int main(int argc, char **argv)
{
    // these are used for after we read in our stuff
    unsigned char prettyPrint = 0, solutionSteps = 0, matrix = 0, path = 0;
    
    // holds the number of rows and columns in our matrix
    int rows = 0, cols = 0;
    
    // sets our default file in and out
    FILE *fileIn = stdin, *fileOut = stdout;
    
    // begins the processing of the flags
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
    
    // reads in our file string
    char *file = readMazeFromFile(fileIn);
    
    // we are done reading in from the file, close it if necessary
    if(fileIn != stdin)
        fclose(fileIn);
    
    /* prints our matrix if we were asked to do so by the user
       there is a new line at the end of our matrix, we don't need an extra */
    if(matrix)
        printf("Read this matrix:\n%s", file);
    
    // determines the number of columns and rows we are dealing with
    cols = getNumCols(file);
    rows = (strlen(file) / (2 * cols));
    
    // process file string to create our maze
    char **maze = processMaze(file, rows, cols);
    
    // file is all done, we can free it here
    free(file);
    file = NULL;
    
    // pretty prints our board if we were asked to do so by user
    if(prettyPrint)
        prettyPrintMaze(maze, rows, cols);

    // empties out the maze
    emptyMaze(maze, rows);
    maze = NULL;
    
    
    // if we need to close the output file we do it right before exit
    if(fileOut != stdout)
        fclose(fileOut);
        
    return EXIT_SUCCESS;
}

