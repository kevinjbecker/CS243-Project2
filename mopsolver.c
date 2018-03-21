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
static char wall = 'O';
static char empty = ' ';


///
/// Function: printHelpMsg
///
/// Description: Prints a help menu when the user asks for it.
///
/// @param *start  The way the program was initially run.
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
           "-i INFILE Read maze from INFILE      (Default: stdin)\n"
           "-o OUTFILE Write maze to OUTFILE     (Default: stdout)\n", start);
}


///
/// Function: getNumCols
///
/// Description: Gets the number of columns in the maze.
///
/// @param *file  The string representation of the maze.
///
/// @return the number of columns in the maze.
///
static size_t getNumCols(const char *fileString)
{
    // index counter, cols counter
    size_t i = 0, cols = 0;
    
    // counts the number of non-space characters before first new line
    while(fileString[i] != '\n')
        if(fileString[i++] != ' ')
            ++cols;
    
    // returns the number of columns we have
    return cols;
}


///
/// Function: hasTrailingSpace
///
/// Description: Determines if there is an extra space at the end of each line.
///
/// @param *fileString  The string representation of the maze.
///
/// @return true if there is a trailing space; false otherwise.
///
static bool hasTrailingSpace(const char *fileString)
{
    // index variable
    size_t i = 0;

    /* we can pre-increment because there will NEVER be a newline at position 0
       searches until we find a new line character */
    while(fileString[++i] != '\n');

    // returns if the character before the new line was a space or not
    return fileString[i-1] == ' ';
}


///
/// Function: createMaze
///
/// Description: Creates a 2d array representing the maze input at the start.
///
/// @param *fileString  The string representation of the maze.
/// @param rows  The number of rows in the maze.
/// @param cols  The number of columns in the maze.
///
/// @return the maze represented as a 2d array of booleans.
///
static bool ** createMaze(const char *fileString,
                          const size_t rows,
                          const size_t cols)
{
    // row/column index counters
    size_t r, c, index;
    
    // our eventual storage location for our maze
    bool **maze = NULL, trailingSpace = hasTrailingSpace(fileString);
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
        {
            // determines the index in the fileString we need to look at
            index = (cols * (r * 2)) + (c * 2);

            // adds r to index if we have a trailing space to account for space
            if(trailingSpace)
                index += r;

            maze[r][c] = (fileString[index] == '0') ? false : true;
        }
    
    // we have finished building our maze we can now return it
    return maze;
}


///
/// Function: clearMaze
///
/// Description: Frees the maze from heapspace.
///
/// @param **maze  The maze to free.
/// @param rows  The number of rows in the maze.
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
/// Function: printEdgeBorder
///
/// Description: Prints the border on the edge of the maze when pretty-printing.
///
/// @param out  The file where the maze should be printed.
/// @param cols  The number of columns in the maze.
///
static void printEdgeBorder(FILE * out, const size_t cols)
{
    // prints our top border
    for(size_t i = 0; i < cols * 2 + 3; ++i)
        fprintf(out, "%c", wall);
    // prints the new line character at the end
    fprintf(out, "\n");
}


///
/// Function: prettyPrintMaze
///
/// Description: Prints the maze in a nice format with a border.
///
/// @param *out  The file where the maze should be printed.
/// @param **maze  The maze to print.
/// @param rows  The number of rows in the maze.
/// @param cols  The number of columns in the maze.
///
static void prettyPrintMaze(FILE * out,
                            bool **maze,
                            const size_t rows,
                            const size_t cols)
{
    // prints our top border
    printEdgeBorder(out, cols);
    
    // goes through and prints each character
    for(size_t r = 0; r < rows; ++r)
    {
        // if r is anything but 0, print a wall (border)
        fprintf(out, "%c", (r) ? wall : empty);
        // prints the maze itself
        for(size_t c = 0; c < cols; ++c)
            fprintf(out, " %c", (maze[r][c]) ? wall : empty);
        // if r is anything but rows-1 print a wall (border)
        fprintf(out, " %c\n", (r != rows-1) ? wall : empty);
    }
    
    // prints our bottom border
    printEdgeBorder(out, cols);
}


///
/// Function: createEmptyVisitedMap
///
/// Description: Creates a 2d array of bools which will be used to keep track of
///              visitation.
///
/// @param rows  The number of rows in the maze.
/// @param cols  The number of columns in the maze.
///
/// @return A 2d array of booleans all set to false.
///
static bool ** createEmptyVisitedMap(const size_t rows, const size_t cols)
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
/// Function: clearVisitedMap
///
/// Description: Frees the visitation map from heapspace.
///
/// @param ** visited  The visitation map to free.
/// @param rows  The number of rows in the maze.
///
static void clearVisitedMap(bool ** visited, const size_t rows)
{
    // contiguously allocates the columns for each row
    for(size_t r = 0; r < rows; ++r)
        free(visited[r]);
        
    // this method is used so that all values are false upon return
    free(visited);
}


///
/// Function: isExit
///
/// Description: Determines if at the exit of the maze.
///
/// @param location  The QNode of the location we are checking.
/// @param rows  The number of rows in the maze.
/// @param cols  The number of columns in the maze.
///
/// @return true if at exit; false otherwise.
///
static bool isExit(QNode location, const size_t rows, const size_t cols)
{
    // if we are at rows-1 and cols-1 we can say we have found the solution
    return (location->row == rows-1 && location->col == cols-1) ? true : false;
}


///
/// Function: getNeighbors
///
/// Description: Gets the neighbors of a certain location in the maze.
///
/// @param maze  The boolean representation of the maze.
/// @param visited  The boolean representation of the visitation maze.
/// @param findFor  The node we are looking to find the neighbors of.
/// @param queue  The queue we will insert neighbors to.
/// @param rows  The number of rows in the maze.
/// @param cols  The number of columns in the maze.
///
static void getNeighbors(bool ** maze,
                         bool ** visited,
                         QNode findFor,
                         Queue queue,
                         const size_t rows,
                         const size_t cols)
{    
    // the location we are searching from
    size_t row = findFor->row, col = findFor->col;
    // the number of steps if node is valid
    size_t numSteps = findFor->steps + 1;
    
    // determines if EAST neighbor is valid and adds it to queue if it is;
    // determined by: valid location, not a wall, and not already visited
    // NOTE: for memory's sake, we mark it as visited here SO THE SAME NODE IS
    //       NOT ADDED MORE THAN ONCE
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
/// Function: findSolution
///
/// Description: Uses BFS to determine the shortest number of steps from start
///              to finish.
///
/// @param **maze  The boolean representation of the maze.
/// @param rows  The number of rows in our maze.
/// @param cols  The number of columns in our maze.
///
/// @return 0 if no path, otherwise the number of steps to get to the exit of
///         the maze.
///
static size_t findSolution(bool ** maze, const size_t rows, const size_t cols)
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
/// Function: main
///
/// Description: Runs an instance of mopsolver.
///
/// @param argc  The number of arguments given upon start.
/// @param ** argv  The string arguments used to launch the program.
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
    bool **maze = createMaze(fileString, rows, cols);
    
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
