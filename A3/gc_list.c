#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "list.h"
#include "gc.h"

void mark_list(void *head) {
	int loop_detect = 0;
	while(head != NULL && loop_detect != 1) {
		loop_detect = mark_one(head);
		head = ((List *)head)->next;
	}
}

void do_mark_and_sweep_list(int sig) { //the handler for SIGUSR1 to call mark_and_sweep
    fprintf(stderr, "\treceived signal %d , now in do_mark_and_sweep_list\n", sig);
    if(sig != SIGUSR1)
        return;
    extern List *ll;
    FILE *fp;
    if((fp = fopen(LOGFILE, "a")) == NULL) {
        perror("File opening");
        exit(1);
    }
    fprintf(fp, "\n\t ****SIGUSR1 RECIEVED FOR LIST****\n");
    fclose(fp);
    mark_and_sweep(ll, mark_list);
}
