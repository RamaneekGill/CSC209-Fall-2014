Assignment A3: Garbage Collection




Home page Course Info Sheet Piazza (DB) PeerWise Anonymous Feedback 
Course Work Assignments Labs MarkUs Lecture Notes 
Grades Tests Forms

Due: Electronically, by 6 p.m. on Monday Nov 10, 2014

Introduction

The first two programming languages we teach at the University of Toronto are Python and Java. Both languages use garbage collection to clean up dynamically allocated memory. This means that programmers don't need to worry about freeing the memory they have allocated. When writing C programs, however, we must explicitly free all dynamically allocated memory when we are finished with it. Since garbage collection plays such an important role in modern high-level languages, it is interesting to studying the implementation and behaviour of garbage collection algorithms.

Mark and sweep is a commonly used garbage collection algorithm, and you will implement a relatively simple version of it for this assignment. Your garbage collector will reclaim memory from two different data structures.

Starter Code

By tomorrow, you will have a many starter code files checked into your a3 directory on your repository. Take some time to read through these files working out what is already provided for you and how your code will fit into the existing framework.

Step 1: Keeping track of dynamically allocated memory

The first step is to write a wrapper function, gc_malloc, around malloc so that we can keep track of dynamically allocated memory. Every time malloc is called, record the address that malloc returned along with a flag for whether that memory reserved at that address is still in use.

You will use a linked list to store every address returned by malloc together with a flag that indicates whether it is still used. Since the memory reference list will be used to track all calls to malloc, you will never need more than one list. Since we need only one list, and since the memory reference list is really a system-level data structure, you should use a global variable to store the head of the list.

Step 2: Mark and Sweep

To be able to tell whether a chunk of dynamically allocated memory is still in use, we must be able to traverse the program's data structures looking for all the memory that is currently accessible, and "marking" the memory chunks that are still reachable. Then we can "sweep" across the linked list of all dynamically allocated memory. Anything not reachable by traversing the program's data structures can be freed.

There are three steps to the mark and sweep algorithm:

Reset: set all of the used flags to "not used" in the memory reference list.
Mark: traverse the program's dynamically allocated data structures and set each address that we reach to "used" in the linked list heap. Note that we need a different mark function for every data structure. Cycles can be avoided by noting when a reference has already been marked as "in use" and halting the traversal of that portion of the data structure.
Sweep: traverse the memory reference list, freeing each address that is not marked as "used", and removing freed nodes from the list.
The file gc.h contains the function prototypes for the external interface of the garbage collector. Note that mark_and_sweep takes a function pointer as an argument. This is the mark function that traverses a specific data structure and knows which components have been dynamically allocated. However, this mark function does not need to know how the garbage collector is implemented, so we provide a mark_one function that will set the address vptr to "in use"

The mark function specific to the linked list should go into a separate file called gc_list.c. Similarly, the mark functions for other data structures should go into similarly named files. You may also want to add header files.

Remember to free memory allocated to store the memory reference list. That memory is not managed by the mark and sweep algorithm.

Sample programs

You will be given two programs on which to implement your garbage collector. These will be committed to your repository. In addition to changing them to use your customized malloc funciton, you need to write their custom "mark" functions.

Linked list

The first program implements a series of operations on a simple linked list. The program mktrans creates a random series of operations on a linked list: adding integer nodes, deleting nodes, and printing the list. The program do_trans takes a file produced by mktrans as a command line argument and executes the operations on a linked list. In the current implementation of do_trans, it periodically calls mark_and_sweep to perform the garbage collection. The files list.h and list.c contain the list operations.

Updating mktrans

In the starter code version, mktrans executes the transactions as it generates them. The output from the list operations is printed to standard output from the functions inside list.c and this is interleaved with the transactions printed from mktrans. One of your jobs is to update mktrans to take a filename as an optional commandline argument. If this argument is provided, mktrans should create a file by this name and write the transactions to this file. The output from the list operations themselves should still go to standard output. If mktrans is run without a commandline argument, it should behave as before, sending all the output to standard out.

The second data-structure: fstree

The second data-structure on which you will implement your garbage collector is based on a file-system tree with directories and subdirectories. In addition to a strict tree hierarchy, it allows links. This means that the fstree can contain cycles. Read the code in fstree.h and fstree.c carefully to understand how it works before you begin to implement the garbage collection.

The program do_fstree.c does a series of transactions on an fstree but doesn't call mark_and_sweep at all yet. You should modify it to periodically call the garbage collector on fstree.

Step 3: Report generation generation and testing

How do you tell whether your garbage collection algorithm is working correctly?

Your garbage collection code will write to a log file summary data each time mark_and_sweep is executed, including the number of chunks of memory that have currently been allocated, and the number of chunks that were freed. You may also include other summary data, and it is up to you to choose a format of displaying this data that makes it easy to read and understand.

Rather than just hard-coding the name of the log file, you will put it in a define constant called LOGFILE. Make gives us the ability to set define constants (macros) by passing them in as an argument to the compiler. Then, in the program you can set a value of LOGFILE if one has not already been set. (See gc.h for the syntactic details.)

You may also want to print out other information while your program is running to help you debug it. It is recommended that you use a global variable to allow you to turn on and off the display of debugging information.

For example:

    int debug = 1; //global variable
    
    // inside some function
    if(debug) {
        printf("Here is something I want to print for debugging purposes.")
    }
Generating some reports doesn't actually tell you whether your program is working correctly. You need to be able to verify that memory that is supposed to be freed actually is freed, and that memory that should not be reclaimed is not, and the all memory allocated my malloc that should be in the memory reference list is in the list.

For this assignment you are not required to write a full test suite, and you are not required to test the sample programs. You are, however, required to provide one or two test programs that run some basic tests on your garbage collector. Your goal is to convince yourself, and the grader that your garbage collection algorithms work in at least some of the basic cases.

Step 4: Triggering garbage collection using signals

After you have the garbage collector working well when it is called periodically from the main program, modify the sample programs so that garbage collection can be triggered at any time by sending SIGUSR1 to the process. This involves creating a signal handler and installing it. You will also need to ensure that the signal does not arrive after you have called gc_malloc, but before the newly element has been inserted into the data structure.

To test that your signal handling function is working, you may want to add some calls to usleep to force the program to run for long enough that you can used the kill program from the command line to send your process the SIGUSR1 signal.

Style and Error checking

You will be marked for programming style and error checking. For programming style, we are looking for readability. Features that make your code more readable are good use of functions to break your program into logical pieces, good use of variable and function names, good use of whitespace, and appropriate level of comments to guide the reader.

For error checking and handling we are primarily checking to make sure that all system calls are checked. A general rule of thumb is that your program should never crash. You may not be able to do anything on encountering an error other than calling exit, but that is fine.

What to submit

Commit all your code to the a3 directory of your repository (including your Makefile, and header files). Please remember to use svn add to add your files before you commit them. Test that you have submitted everything that you intended to by checking out your repository into another empty directory, and rerun your tests. If you don't submit your files, we have to give you 0.

Code that does not compile on cdf using your submitted Makefile will receive a grade of 0. So don't write down everything and then try to get it to compile and run. Write a function, or even a placeholder for a function, then compile and run. Work on the program incrementally, and keep testing it in pieces. When you have something working, commit it. For example, after you have written gc_malloc, run it and test it. Convince yourself that it works before moving to the next step.

References

Here are several of references that go into more detail about the differences between various algorithms for garbage collections. You may find it useful to read some of the descriptions of mark and sweep.

The Very Basics of Garbage Collection
Wikipedia on Garbage Collection
Back To Basics: Mark and Sweep Garbage Collection