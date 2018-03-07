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
#include <stdlib.h>

// our Queue node system
typedef struct qnode_s{
    QNode *next;
    size_t x, y; // the x and y coordinates
} QNode;

// used for reading in the data
typedef struct line_s{
    Line *child;
    char *data;
} Line;


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
    exit(EXIT_FAILURE);
}





void addLine(Line **root, size_t size, char *line)
{
    Line *lowest = NULL;
    lowest = findLowest(*root);
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
                if(fileOut == NULL)
                    perror("Error opening output file");
                break;
        }
    }
    
    printf("starting point: prettyPrint=%d, solutionSteps=%d, matrix=%d, "
           "path=%d\n", prettyPrint, solutionSteps, matrix, path);

    // -1 so we can determine what to do later
    int numLines = 0;
    char *line = NULL;
    size_t linesize = 0;
    
    // some temporary heap 2d char array
    // used so that we can read in the entire file to memory and work from there
    
    Line *maze = NULL;
    maze->next = NULL;
    
    while(getline(&line, &linesize, fileIn) > 0)
    {
        addLine(&maze, line, 
        ++numLines;
    }

    return EXIT_SUCCESS;
}

