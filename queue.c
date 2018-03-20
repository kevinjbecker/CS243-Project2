///
/// File: queue.c
///
/// Description: An abstract data type for a queue module.
///
/// @author kjb2503 : Kevin Becker
///
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#include <stdlib.h> // malloc, free
#include <stdbool.h> // boolean data members
#include <assert.h> // used for the assert in que_destroy(1)
#include "queue.h" // queue functions and structures


/// creates an empty queue with comparison function pointer passed as argument
Queue que_create()
{
    // pointer to a new queue (set to NULL for now)
    Queue queue = NULL;
    // allocates enough space for our queue
    queue = malloc(sizeof(struct queue_s));
    
    // if we encounter an error in creating our queue
    if(queue == NULL)
        return queue;

    // sets our first and last nodes to null
    queue->firstNode = NULL;
    queue->lastNode = NULL;
    // current size is 0
    queue->size = 0;

    // returns our new, empty queue
    return queue;
}


/// clears a queue of all data, freeing all blocks of allocated memory
void que_clear( Queue queue )
{
    // we only want to do this if we have an actual queue and it's not empty
    if (queue != NULL && !que_empty(queue))
    {
        // when we get here we are guaranteed that there is a firstNode node
        // declares two nodes: one definitely has data, the other might not
        QNode curNode = queue->firstNode, nextNode = curNode->behind;

        // keep freeing until our nextNode is NULL
        while (nextNode != NULL)
        {
            // frees our current node
            free(curNode);
            // sets our current node to next
            curNode = nextNode;
            // sets next node to the new current's behind node
            nextNode = curNode->behind;
        }

        // clears the last node from heap
        // this is required since we don't free the last node in the loop
        free(curNode);
        // sets our curNode to NULL (nextNode is already NULL)
        curNode = NULL;

        // last thing we do is set our queue's number polled
        // to 0 and our first nodes to NULL, it's now empty
        queue->size = 0;
        queue->firstNode = NULL;
        queue->lastNode = NULL;
    }
}


/// destroys the queue by clearing all data and freeing the queue from memory
void que_destroy( Queue queue )
{
    // clears all the data out of the queue
    // why implement the same thing twice?
    que_clear(queue);

    // in order to free our queue we need to have an empty queue
    assert(que_empty(queue));

    // frees our queue (only difference for destroy function)
    free(queue);

    // sets queue to NULL
    queue = NULL;
}


/// inserts our coordinates
void que_insert( Queue queue, size_t x, size_t y, int steps )
{
    // creates a new node with data of data
    QNode newNode = NULL;
    // allocates enough space for our QNode on the heap
    newNode = malloc(sizeof(struct qnode_s));
    // sets our data in place
    newNode->x = x;
    newNode->y = y;
    newNode->steps = steps;
    // our new QNode might have something behind it, but we don't know
    newNode->behind = NULL;

    // if we have an empty queue we can immediately just put the node in
    // (no comparisons are needed)
    if(queue->size == 0)
    {
        // sets both the first and last element to new node
        queue->firstNode = newNode;
        queue->lastNode = newNode;

        // checks to make sure we have the
        assert(queue->lastNode == newNode);
    }
    // FIFO
    else
    {
        // last node's next member is set to our new node
        queue->lastNode->behind = newNode;
        // lastNode of queue set to our new node
        queue->lastNode = newNode;
        // checks to make sure we have properly done this
        assert(queue->lastNode == newNode);
    }

    // adds one to the size of the queue (we just added one in)
    queue->size++;
}


/// removes the first element in our queue and frees the memory
QNode que_remove( Queue queue )
{
    // ( lastNode ) QNode -> QNode -> QNode -> QNode ( firstNode )
    // pulls in our firstNode node
    QNode firstNode = queue->firstNode;
    
    // changes our firstNode node to be the behind member
    queue->firstNode = firstNode->behind;
    
    // if we're on our last node (size will be 0 next, we need to keep track of
    // that fact and set our lastNode to NULL)
    if (queue->size == 1)
        queue->lastNode = NULL;
    // subtracts one from the size
    queue->size--;

    // returns the value we just removed
    return firstNode;
}


/// returns if the queue is empty or not
bool que_empty( Queue queue )
{
    return (queue->size == 0) ? true : false;
}
