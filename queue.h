/// File: $Id: queue.h,v 1.5 2018/03/19 19:45:39 csci243 Exp $
///
/// Description:  Interface to the Queue module
///
/// @author:  wrc
/// @author kjb2503 : Kevin Becker
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>

//
// Only define the Queue type if we are _not_ compiling the
// module's implementation file.
//
// In the implementation file, define the Queue type as you need
// to for the implementation, and then define the symbol _QUEUE_IMPL_
// immediately before including this header file to prevent a
// redefinition of the type.
//

// our storage unit
typedef struct qnode_s{
    struct qnode_s *behind;
    size_t row, col, steps; // the x and y coordinates and steps to location
} * QNode;

// Queue structure
typedef struct queue_s{
    // the front of our queue (can be NULL if empty queue)
    QNode firstNode;
    // the back of our queue (can be NULL if empty queue)
    QNode lastNode;
    // the size of our queue
    size_t size;
} * Queue;

/// 
/// Create a Queue routine.
///
/// @return a Queue instance, or NULL if the allocation fails
Queue que_create();

/// Tear down and deallocate the supplied Queue.
///
/// @param queue - the Queue to be manipulated
void que_destroy( Queue queue );

/// Remove all contents from the supplied Queue.
///
/// @param queue - the Queue to be manipuated
void que_clear( Queue queue );

/// Insert the specified data into the Queue in the appropriate place
///
/// Uses the queue's comparison function to determine the appropriate
/// place for the insertion.
///
/// @param queue the Queue into which the value is to be inserted
/// @param data the data to be inserted
/// @exception If the value cannot be inserted into the queue for
///     whatever reason (usually implementation-dependent), the program
///     should terminate with an error message.  This can be done by
///     printing an appropriate message to the standard error output and
///     then exiting with EXIT_FAILURE, or by having an assert() fail.
void que_insert( Queue queue, size_t row, size_t col, size_t steps );

/// Remove and return the first element from the Queue.
///
/// @param queue the Queue to be manipulated
/// @return the value that was removed from the queue. INFO ABOUT RETURN: nodes
///         are stored on the heap so you MUST free them after done using it.
/// @exception If the queue is empty, the program should terminate
///     with an error message.  This can be done by printing an
///     appropriate message to the standard error output and then
///     exiting with EXIT_FAILURE, or by having an assert() fail.
QNode que_remove( Queue queue );

/// Indicate whether or not the supplied Queue is empty
///
/// @param the Queue to be tested
/// @return true if the queue is empty, otherwise false
bool que_empty( Queue queue );

#endif

