#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fstree.h"
#include "gc.h"

void mark_fstree(void *root) {
	//printf("in mark_fstree\n");
	if(root == NULL)
		return;

	int loop_detect = 0;
	loop_detect = mark_one(root);
	mark_one(((Fstree *)root)->name);
	Link *links = ((Fstree *)root)->links;
	while(links != NULL && loop_detect != 1) {
		mark_one(links);
		mark_fstree(links->fptr);
		links = links->next;
	}
}

void do_mark_and_sweep_fstree(int sig) { //the handler for SIGUSR1 to call mark_and_sweep
    fprintf(stderr, "\treceived signal %d , now in do_mark_and_sweep_fstree\n", sig);
    extern Fstree *root;
    FILE *fp;
    if((fp = fopen(LOGFILE, "a")) == NULL) {
        perror("File opening");
        exit(1);
    }
    fprintf(fp, "\n\t ****SIGUSR1 RECIEVED FOR FS_TREE****\n");
    fclose(fp);
    mark_and_sweep(root, mark_fstree);
}
