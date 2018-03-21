///
/// File: mopsolver.c
///
/// Description: Takes a maze "construction" file as input and attempts to find
///              the shortest distance from start to finish.
///
/// @author kjb2503 : Kevin Becker
///
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _GNU_SOURCE
#include <unistd.h> // getopt
#include <stdio.h> // printing
#include <stdbool.h> // boolean items
#include <string.h> // string functions
#include <stdlib.h> // allocation functions
#include "fileRead.h" // reading in the file
#include "queue.h" // queue related items

// used in our pretty-print function
#define WALL 'O'
#define EMPTY ' '


///
/// Function: printHelpMsg
///
/// Description: Prints a help menu when the user asks for it.
///
/// @param start  The way the program was initially run.
///
static void printHelpMsg(char *start)
{
    // prints usage and exits
    printf("Usage:\n"
           "%s [-hbsm] [-i INFILE] [-o OUTFILE]\n\n"
           "Options:\n"
           "-h Prints this message to stdout and exits.\n"
           "-b Add borders and pretty-print.     (Default: off)\n"
           "-s Add shortest solution step total. (Default: off)\n"
           "-m Print matrix after reading.       (Default: off)\n"
           //"-p Print solution with path.         (Default: off)\n"
           "-i INFILE Read maze from INFILE      (Default: stdin)\n"
           "-o OUTFILE Write maze to OUTFILE     (Default: stdout)\n", start);
}


///
/// Function:
///
/// Description:
///
///
///
static size_t getNumCols(const char *file)
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


///
/// Function:
///
/// Description:
///
///
///
static bool ** processMazeFromFileString(const char *fileString,
                           const size_t rows, 
                           const size_t cols)
{
    // row/column index counters
    size_t r, c;
    
    // our eventual storage location for our maze
    bool **maze = NULL;
    // contiguously allocates the right number of rows
    maze = calloc(sizeof(bool *), rows);
    
    // contiguously allocates the columns for each row
    for(r = 0; r < rows; ++r)
        maze[r] = calloc(sizeof(bool), cols);
    
    /* goes through each row and column and sets it to the proper value based
       on the file string 
       NOTE: the index in the file string is: (cols * row * 2) + (col * 2)
             effectively this treats the 1d array as a 2d one :) */
    for(r = 0; r < rows; ++r)
        for(c = 0; c < cols; ++c)
            maze[r][c] = (fileString[(cols * (r * 2)) + (c * 2)] == '0') ? false : true;
    
    // we have finished building our maze we can now return it
    return maze;
}


///
/// Function:
///
/// Description:
///
///
///
static void clearMaze(bool **maze, const size_t rows)
{
    // frees each individual row
    for(size_t r = 0; r < rows; ++r)
        free(maze[r]);
    
    // finally frees our maze
    free(maze);
}


///
/// Function:
///
/// Description:
///
///
///
static void printEdgeBorder(FILE * out, const size_t cols)
{
    // prints our top border
    for(size_t i = 0; i < cols * 2 + 3; ++i)
        fprintf(out, "%c", WALL);
    // prints the new line character at the end
    fprintf(out, "\n");
}


///
/// Function:
///
/// Description:
///
///
///
static void prettyPrintMaze(FILE * out, bool **maze, const size_t rows, const size_t cols)
{
    // prints our top border
    printEdgeBorder(out, cols);
    
    // goes through and prints each character
    for(size_t r = 0; r < rows; ++r)
    {
        // if r is anything but 0, print a wall (border)
        fprintf(out, "%c", (r) ? WALL : EMPTY);
        // prints the maze itself
        for(size_t c = 0; c < cols; ++c)
            fprintf(out, " %c", (maze[r][c]) ? WALL : EMPTY);
        // if r is anything but rows-1 print a wall (border)
        fprintf(out, " %c\n", (r != rows-1) ? WALL : EMPTY);
    }
    
    // prints our bottom border
    printEdgeBorder(out, cols);
}


///
/// Function:
///
/// Description:
///
///
///
static bool ** createEmptyVisitedMap(size_t rows, size_t cols)
{
    // used to keep track of our visited nodes
    bool ** visited = NULL;
    
    // this method is used so that all values are false upon return
    visited = calloc(sizeof(bool *), rows);
    
    // contiguously allocates the columns for each row
    for(size_t r = 0; r < rows; ++r)
        visited[r] = calloc(sizeof(bool), cols);
    
    // returns new visited map with all values of FALSE
    return visited;
}


///
/// Function:
///
/// Description:
///
///
///
static void clearVisitedMap(bool ** visited, size_t rows)
{
    // contiguously allocates the columns for each row
    for(size_t r = 0; r < rows; ++r)
        free(visited[r]);
        
    // this method is used so that all values are false upon return
    free(visited);
}


///
/// Function:
///
/// Description:
///
///
///
static bool isExit(QNode location, size_t rows, size_t cols)
{
    // if we are at rows-1 and cols-1 we can say we have found the solution
    return (location->row == rows-1 && location->col == cols-1) ? true : false;
}


///
/// Function:
///
/// Description:
///
///
///
static void getNeighbors(bool ** maze, bool ** visited, QNode findFor, 
                         Queue queue, size_t rows, size_t cols)
{    
    // the location we are searching from
    size_t row = findFor->row, col = findFor->col;
    // the number of steps if node is valid
    size_t numSteps = findFor->steps + 1;
    
    // determines if EAST neighbor is valid and adds it to queue if it is;
    // determined by: valid location, not a wall, and not already visited
    // NOTE: for memory's sake, we mark it as visited here SO THE SAME NODE ISN'T
    //       ADDED MORE THAN ONCE.  Saves memory AND time.
    if(col + 1 < cols && !maze[row][col+1] && !visited[row][col+1])
    {
        que_insert(queue, row, col+1, numSteps);
        visited[row][col+1] = true;
    }
    // SOUTH...
    if(row + 1 < rows && !maze[row+1][col] && !visited[row+1][col])
    {
        que_insert(queue, row+1, col, numSteps);
        visited[row+1][col] = true;
    }
    // WEST...
    if(col > 0 && !maze[row][col-1] && !visited[row][col-1])
    {
        que_insert(queue, row, col-1, numSteps);
        visited[row][col-1] = true;
    }
    // NORTH...
    if(row > 0 && !maze[row-1][col] && !visited[row-1][col])
    {
        que_insert(queue, row-1, col, numSteps);
        visited[row-1][col] = true;
    }
}


///
/// Function:
///
/// Description:
///
///
///
static size_t findSolution(bool ** maze, size_t rows, size_t cols)
{
    /* we first check that the last and first spaces are open
       waste of time if we can't get in/out of the maze */
    if(maze[rows-1][cols-1] || maze[0][0])
        return 0;

    // the number of steps
    size_t steps = 0;

    // the node currently being searched
    QNode searching = NULL;

    // the visitation map (true is visited, false otherwise)
    bool ** visited = NULL;

    // creates a new queue here which will be used for BFS
    // the queue of nodes left to search
    Queue q = que_create();
    
    // inserts a node at (0,0) with 1 step (we must step into the maze first)
    que_insert(q, 0, 0, 1);
    
    // used to keep track of visited nodes    
    visited = createEmptyVisitedMap(rows, cols);

    // keeps going while we still have queue nodes
    while(!que_empty(q))
    {
        // removes the next QNode
        searching = que_remove(q);
        
        // if we are at solution we need to teardown
        if(isExit(searching, rows, cols))
        {
            // sets our number of steps and breaks the loop
            steps = searching->steps;
            break;
        }
        // gets our valid neighbors and adds them to the queue if not at exit
        else
            getNeighbors(maze, visited, searching, q, rows, cols);
        
        // we're done with searching, we can free it
        free(searching);
        searching = NULL;
    }
    
    // frees our searching node if it isn't NULL
    if(searching != NULL)
    {
        free(searching);
        searching = NULL;
    }
    
    /* destroys the remaining queue (we don't care, we've found shortest path)
       might be empty already but hey that's okay */
    que_destroy(q);
    q = NULL;
    
    
    // clears our matrix
    clearVisitedMap(visited, rows);
    visited = NULL;

    /* if steps is STILL 0 here we have run out of spaces to inspect and there
       is no solution */
    return steps;
}


///
/// Function:
///
/// Description:
///
///
///
int main(int argc, char **argv)
{
    // these are used for after we read in our stuff
    unsigned char prettyPrint = 0, solutionSteps = 0, matrix = 0;
    
    // holds the number of rows and columns in our matrix
    size_t rows = 0, cols = 0, steps = 0;
    
    // sets our default file in and out
    FILE *fileIn = stdin, *fileOut = stdout;
    
    // used for processing the flags
    int opt;
    
    // processes our flags (if any are present)
    while((opt = getopt(argc, argv, "hbsmi:o:")) != -1)
    {
        switch(opt)
        {
            // help menu and quit
            case 'h':
                printHelpMsg(argv[0]);
                return EXIT_SUCCESS;
            // flag which will add border and pretty print the read in maze
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
            // flag preset to set our fileIn
            case 'i':
                // opens the in file in read-only mode
                fileIn = fopen(optarg, "r");
                /* if the file doesn't exist (i.e. fopen returns NULL), print
                   the error and exit */
                if(fileIn == NULL)
                {
                    perror("Error opening input file");
                    return EXIT_FAILURE;
                }
                break;
            case 'o':
                /* open our output file in w+ mode which will create the file
                   if it doesn't exist and overwrite if it does */
                fileOut = fopen(optarg, "w+");
                // if we have an error we report it here and exit
                if(fileOut == NULL)
                {
                    perror("Error opening output file");
                    return EXIT_FAILURE;
                }
                break;
        }
    }

    /* gets our file as a string (found in fildRead.c)
       NOTE: we can read a lot faster if we are reading from a file; we must be
             much more careful when reading from stdin */
    char *fileString = getFileAsString(fileIn);

    // if there is no maze to build from, we need to exit now!
    if(strlen(fileString) == 0)
    {
        printf("No maze specified.\n");
        return EXIT_FAILURE;
    }
    
    // we are done reading in from the file at this point, close it if necessary
    if(fileIn != stdin)
        fclose(fileIn);
    
    /* prints our matrix if we were asked to do so by the user
       there is a new line at the end of our matrix, we don't need an extra */
    if(matrix)
        fprintf(fileOut, "Read this matrix:\n%s", fileString);

    // determines the number of columns and rows we are dealing with
    cols = getNumCols(fileString);
    /* rows is the length of the string divided by twice the number of cols
       this is because there are spaces or new lines separating each column, 
       thus we need to account for that. */
    rows = (strlen(fileString) / (2 * cols));
    
    // process file string to create our maze--a boolean matrix
    bool **maze = processMazeFromFileString(fileString, rows, cols);
    
    // file is all done, we can free it here and set file to NULL
    free(fileString);
    fileString = NULL;

    // if the user wants the number of steps to find solution, print that now
    if(solutionSteps)
    {
        /* steps is set to the return of findSolution which returns the number
           of steps in the shortest path */
        steps = findSolution(maze, rows, cols);

        
        // if steps is not -1 (a.k.a. there WAS a path), that is returned here.
        if (steps > 0)
            fprintf(fileOut, "Solution in %zu steps.\n", steps);
        else
            fprintf(fileOut, "No solution.\n");
    }

    // pretty prints our board if we were asked to do so by user
    if(prettyPrint)
        prettyPrintMaze(fileOut, maze, rows, cols);

    // empties out the maze since it is done
    clearMaze(maze, rows);
    maze = NULL;
    
    // if we need to close the output file we do it right before exit
    if(fileOut != stdout)
        fclose(fileOut);
        
    // lastly we need to return that we have successfully run the program
    return EXIT_SUCCESS;
}

