/* Read and execute a list of operations on a linked list.
 * Periodically call the garbage collector.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include "gc.h"
#include "gc_list.h"

#define MAX_LINE 128
#define ADD_NODE 1
#define DEL_NODE 2
#define PRINT_LIST 3

void mark_list(void *);

int main(int argc, char **argv) {
    printf("PID OF DO_TRANS RUNNING IS: %d\n", getpid());
    char line[MAX_LINE];
    char *str;

    if(argc != 2) {
        fprintf(stderr, "Usage: do_trans filename\n");
        exit(1);
    }

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    extern List *ll;
    int count = 1;

    //activate handler when recieve SIGUSR1
    signal(SIGUSR1, do_mark_and_sweep_list);

    //for signal blocking SIGUSR1 to prevent mark_and_sweep between mallocs
    sigset_t sigs, old_sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGUSR1);

    while(fgets(line, MAX_LINE, fp) != NULL) {
        char *next;
        int value;
        int type = strtol(line, &next, 0);

        //now lets block SIGUSR1 in functions where malloc is called
        switch(type) {
            case ADD_NODE :
                value = strtol(next, NULL, 0);
                block_SIGUSR1(&sigs, &old_sigs);
                ll = add_node(ll, value);
                rm_block_SIGUSR1(&old_sigs);
                break;
            case DEL_NODE :
                value = strtol(next, NULL, 0);
                ll = remove_node(ll, value);
                break;
            case PRINT_LIST :
                //need to block since toString uses malloc
                block_SIGUSR1(&sigs, &old_sigs);
                str = tostring(ll);
                rm_block_SIGUSR1(&old_sigs);
                printf("List is %s\n", str);
                break;
            default :
                fprintf(stderr, "Error: bad transaction type\n");

        }
    
        
        if(count % 10 == 0) {
            //printf("About to mark_and_sweep\n");
            
            //going to mark_and_sweep by sending a signal
            //this shows that my mark_and_sweep works
            //and my signal listener works for SIGUSR1
            kill(getpid(), SIGUSR1);
            sleep(3);

            //char check[MAX_LINE];
            //fgets(check, MAX_LINE, stdin);

/*
    As you can see the code works flawlessly for alll
    list operations. By looking at gc.log after this is
    program is run you can see that the memory is being 
    freed correctly. This works flawlessy even for a very
    long test file (test it with trans to see)

    My gc.log will report everytime mark_and_sweep runs:
        whether it was called by a signal or not. 
        how many mem_chunks there are
        how many gc_mallocs were called
        how many mem_chunks and gc_malloc calls are being freed
        the address of each mem_chunk/gc_malloc being freed

    Then it will print a list of the address of each mem_chunk
    and what it points to.

    You can look at the binary's to see how many mem_chunks are
    required/freed after each do_transaction loop and compare it
    with the output on gc.log to see that its correct.

    Keep in mind a ll node only needs one gc_malloc
    */

        }       
        count++;

    }
    //print_mem_list(NULL);
    return 0;
}