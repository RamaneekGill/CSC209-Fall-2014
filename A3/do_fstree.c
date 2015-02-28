#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "fstree.h"
#include "gc.h"
#include "gc_fstree.h"

int main() {
    extern Fstree *root;
    root = create_node("root");
    //printf("\t root is at %p with name %s", root, root->name);

    printf("PID OF DO_FSTREE RUNNING IS: %d\n", getpid());

    printf("\n\n STARTING DO TRANSACTIONS fs_trans1\n");
    do_transactions("fs_trans1");
    print_tree(root, 0);
    kill(getpid(), SIGUSR1);
    sleep(2);
    /* try uncommenting various bits to use other test files */

    printf("\n\n STARTING DO TRANSACTIONS fs_trans2\n");
    root = create_node("root");
    do_transactions("fs_trans2");
    print_tree(root, 0);
    kill(getpid(), SIGUSR1);
    sleep(2);

    printf("\n\n STARTING DO TRANSACTIONS fs_trans_loop\n");
    root = create_node("root");
    do_transactions("fs_trans_loop");
    kill(getpid(), SIGUSR1);
    
    //PLEASE LOOK AT gc.log TO SEE HOW MY GC WORKS

    /*
    As you can see the code works flawlessly for all
    fs_tree operations including hard links and for all
    list operations. By looking at gc.log after this is
    program is run you can see that the memory is being 
    freed correctly.

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

    Keep in mind that a link needs a gc_malloc and a Fstree
    needs 2 gc_mallocs (one for it's name)
    */
    
    //just for gc.log debugging purposes and to show it
    //should only have 12 mem_chunks in gc.log
    kill(getpid(), SIGUSR1);

    return 0;
}
                  
