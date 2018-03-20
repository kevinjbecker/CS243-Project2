///
/// File: mopsolver.c
///
/// Description: Takes a maze "construction" file as input and attempts to find
///              the shortest distance from start to finish.
///
/// @author kjb2503 : Kevin Becker
// // // // // // // // // // // // // // // // // // // // // // // // // // //

// TODO: make sure all references are actually using the correct x and y
// x => col
// y => row

#define _DEFAULT_SOURCE
#include <unistd.h> // getopt
#include <stdio.h> // printing
#include <stdbool.h> // boolean items
#include <string.h> // string functions
#include <stdlib.h> // allocation functions
#include "queue.h" // queue related items

// used in our pretty-print function
#define WALL 'O'
#define EMPTY ' '


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
           //"-p Print solution with path.         (Default: off)\n"
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


static bool ** processMaze(const char *fileString,
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
       NOTE: the index in the file string is: (row * rows * 2) + (col * 2) */
    for(r = 0; r < rows; ++r)
        for(c = 0; c < cols; ++c)
            maze[r][c] = (fileString[(rows * (r * 2)) + (c * 2)] == '0') ? false : true;
    
    // we have finished building our maze we can now return it
    return maze;
}

static void emptyMaze(bool **maze, const size_t rows)
{
    // frees each individual row
    for(size_t r = 0; r < rows; ++r)
        free(maze[r]);
    
    // finally frees our maze
    free(maze);
}


static void printEdgeBorder(const size_t cols)
{
    // prints our top border
    for(size_t i = 0; i < cols * 2 + 3; ++i)
        printf("%c", WALL);
    // prints the new line character at the end
    printf("\n");
}


static void prettyPrintMaze(FILE * out, bool **maze, const size_t rows, const size_t cols)
{
    // prints our top border
    printEdgeBorder(cols);
    
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
    printEdgeBorder(cols);
}


static char * readMazeFromFile(FILE *fileIn)
{
    // the line buffer getline will write to
    char *buf = NULL;
    // the line size which getline will change
    size_t bufsize = 0;
    // allocates our empty file string
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


static bool isExit(QNode location, size_t rows, size_t cols)
{
    // if we are at rows-1 and cols-1 we can say we have found the solution
    return (location->x == rows-1 && location->y == cols-1) ? true : false;
}


static void getNeighbors(bool ** maze, bool ** visited, QNode findFor, 
                         Queue queue, size_t rows, size_t cols)
{
    // from location S
    //                  - <- (x - 1, y)
    //  (x, y - 1) -> - S - <- (x, y + 1)
    //    (x + 1, y) -> -  
    
    // the location we are searching from
    size_t x = findFor->x, y = findFor->y;
    // the number of steps if node is valid
    int numSteps = findFor->steps + 1;
    
    // determines if EAST neighbor is valid and adds it to queue if it is
    if(y + 1 < cols && !maze[x][y+1] && !visited[x][y+1])
        que_insert(queue, x, y+1, numSteps);
    // SOUTH...
    if(x + 1 < rows && !maze[x+1][y] && !visited[x+1][y])
        que_insert(queue, x+1, y, numSteps);
    // WEST...
    if(y != 0 && !maze[x][y-1] && !visited[x][y-1])
        que_insert(queue, x, y-1, numSteps);
    // NORTH...
    if(x != 0 && !maze[x-1][y] && !visited[x-1][y])
        que_insert(queue, x-1, y, numSteps);
}


static int findSolution(bool ** maze, size_t rows, size_t cols)
{
    // we first check that the last and first spaces are open
    // waste of time if we can't get in/out of the maze
    if(maze[rows-1][cols-1] || maze[0][0])
        return -1;

    int steps = -1;
    // the queue of nodes left to search
    Queue q;
    // the node currently being searched
    QNode searching;
    // the visitation map (true is visisted, false otherwise)
    bool ** visited;

    // creates a new queue here which will be used for BFS
    q = que_create();
    
    // inserts a node at (0,0) with 1 step
    que_insert(q, 0, 0, 1);
    
    // used to keep track of visited nodes    
    visited = createEmptyVisitedMap(rows, cols);
    
    // maybe throw into bfs function?
    // keeps going while we still have queue space
    while(!que_empty(q))
    {
        // removes the next QNode
        searching = que_remove(q);
        
        // marks point x , y as visited
        visited[searching->y][searching->x] = true;
        
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
    }
    
    // frees our searching node
    free(searching);
    searching = NULL;
    
    // destroys the remaining queue (we don't care, we've found shortest path)
    // might be empty already but hey that's okay
    que_destroy(q);
    q = NULL;

    // if steps is STILL -1 here we have run out of spaces to inspect and there
    // is no solution
    return steps;
}


int main(int argc, char **argv)
{
    // these are used for after we read in our stuff
    unsigned char prettyPrint = 0, solutionSteps = 0, matrix = 0, path = 0;
    
    // holds the number of rows and columns in our matrix
    size_t rows = 0, cols = 0;
    
    int steps = -1;
    
    // sets our default file in and out
    FILE *fileIn = stdin, *fileOut = stdout;
    
    // begins the processing of the flags
    int opt;
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
            // flag which will print our solution with path
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
        fprintf(fileOut, "Read this matrix:\n%s", file);
    
    // determines the number of columns and rows we are dealing with
    cols = getNumCols(file);
    rows = (strlen(file) / (2 * cols));
    
    // process file string to create our maze
    bool **maze = processMaze(file, rows, cols);
    
    // file is all done, we can free it here and set file to NULL
    free(file);
    file = NULL;

    // if the user wants the number of steps to find solution, print that now
    if(solutionSteps)
    {
        steps = findSolution(maze, rows, cols);
        
        // wish this would work with ternary...
        if (steps != -1)
            fprintf(fileOut, "Found solution in %d steps\n", steps);
        else
            fprintf(fileOut, "No solution.\n");
    }

    // pretty prints our board if we were asked to do so by user
    if(prettyPrint)
        prettyPrintMaze(fileOut, maze, rows, cols);

    // empties out the maze since it is done
    emptyMaze(maze, rows);
    maze = NULL;
    
    
    // if we need to close the output file we do it right before exit
    if(fileOut != stdout)
        fclose(fileOut);
        
    return EXIT_SUCCESS;
}

