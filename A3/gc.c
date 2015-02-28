#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h> 
#include "gc.h"
#include "gc_fstree.h"
#include "gc_list.h"


//Global variable for the head of linked list of malloc calls
//The head of the list is always the most recently created address
struct mem_chunk *head_mem_list = NULL;
struct mem_chunk *curr_mem_chunk;

void *gc_malloc(int nbytes) {
	void *malloc_ptr = malloc(nbytes);
	if (malloc_ptr == NULL) {
		perror("malloc");
		return malloc_ptr;
	}

	//printf("\t\t\tHead of memlist is %p About to gc_malloc %p\n", head_mem_list, malloc_ptr);

	//If head is empty
	if (head_mem_list == NULL) {
		head_mem_list = malloc(sizeof(struct mem_chunk));
		if (head_mem_list == NULL) {
			perror("malloc for head_mem_list");
			exit(1);
		}
		head_mem_list->in_use = USED;
		head_mem_list->address = malloc_ptr;
		head_mem_list->next = NULL;
	} else {
		//printf("\t\tin else block\n");
		struct mem_chunk *mem_node;
		if((mem_node = malloc(sizeof(struct mem_chunk))) == NULL) {
			perror("malloc for mem_chunk");
			exit(1);
		}
		//printf("\t\t !!!!!!finished mallocing new mem_chunk\n");
		mem_node->in_use = USED;
		mem_node->address = malloc_ptr;
		mem_node->next = head_mem_list;
		head_mem_list = mem_node;
	}
	//printf("\t\t\tHead of memlist is now %p Finished gc_malloc \n", head_mem_list);
	
	//print_mem_list(NULL);

	return malloc_ptr;
}

void mark_and_sweep(void *obj, void (*mark_obj)(void *)) {
	FILE *fp;
	if((fp = fopen(LOGFILE, "a")) == NULL) {
		perror("File opening");
		exit(1);
	}

	fprintf(fp, "\n~~~~~~~~~~~About to run mark_and_sweep~~~~~~~~~~~\n");

	int num_mem_chunks = 0;
	int num_mem_chunks_freed = 0;
	
	//set each mem_chunk to NOT_USED
	curr_mem_chunk = head_mem_list;
	
	while(curr_mem_chunk != NULL) {
		num_mem_chunks++;
		curr_mem_chunk->in_use = NOT_USED;
		curr_mem_chunk = curr_mem_chunk->next;
	}

	fprintf(fp, "Number of mem_chunk's that are allocated: %d\n\n", num_mem_chunks);

	mark_obj(obj); //mark the list

	//now lets free memory that's not in use
	curr_mem_chunk = head_mem_list;
	struct mem_chunk *prev_mem_chunk = curr_mem_chunk;


	while(curr_mem_chunk != NULL) { //loop through each mem_chunk in list

		if(curr_mem_chunk->in_use == NOT_USED) { //removing a node
			//printf("\t\tabout to free memchunk %p and node %p \n", curr_mem_chunk, curr_mem_chunk->address);
			
			//2 because each time this if block is reached 2 mem_chunks will be freed
			//1 ll node/fstree node/fstree link/fstree name will be freed
			//and its corresponding mem_chunk that keeps track of it
			num_mem_chunks_freed = num_mem_chunks_freed + 2;

			fprintf(fp, "About to free memory at %p\n", curr_mem_chunk->address);
			free(curr_mem_chunk->address); //free the ll node

			if(head_mem_list == curr_mem_chunk) {
				//remove the mem_chunk and update the head pointer
				head_mem_list = head_mem_list->next;
				prev_mem_chunk = curr_mem_chunk->next;

				fprintf(fp, "About to free mem_chunk at %p\n", curr_mem_chunk);
				free(curr_mem_chunk);

				curr_mem_chunk = prev_mem_chunk;
				//now prev and curr chunk point to same thing
				//so the loop starts as again as if it just begun
				continue;
			}

			prev_mem_chunk->next = curr_mem_chunk->next; //remove mem_chunk from list

			fprintf(fp, "About to free mem_chunk at %p\n", curr_mem_chunk);
			free(curr_mem_chunk); //free the mem_chunk that's not needed anymore

			curr_mem_chunk = prev_mem_chunk->next; //update the curr_mem_chunk
		} 

		else { //not removing a node
			prev_mem_chunk = curr_mem_chunk;
			curr_mem_chunk = curr_mem_chunk->next;
		}
	}

	fprintf(fp, "\nNumber of nodes that were freed: %d\n", num_mem_chunks_freed/2);
	fprintf(fp, "Number of mem_chunk's that were freed: %d\n", num_mem_chunks_freed/2);
	fprintf(fp, "Number of mem_chunk's that remain: %d\n\n", num_mem_chunks - num_mem_chunks_freed/2);
	print_mem_list(fp);
	fprintf(fp, "\n~~~~~~~~~~~~~~~~FINISHED SWEEPING~~~~~~~~~~~~~~~~\n");
	if(fclose(fp) != 0) { 
		perror("fclose");
		exit(1);
	}
}

int mark_one(void *vptr) {
	curr_mem_chunk = head_mem_list;
	while(curr_mem_chunk->address != vptr && curr_mem_chunk!= NULL) { //in the mem_list
		curr_mem_chunk = curr_mem_chunk->next;
	}
	
	//now curr_mem_chunk points to the same address as vptr
	/*
	if(curr_mem_chunk->address != vptr)
		printf("\t\tmarking %p, it doesn''t match %p? \n", curr_mem_chunk->address, vptr);
	*/
	if(curr_mem_chunk->in_use == USED || curr_mem_chunk->address != vptr)
		return 1;

	curr_mem_chunk->in_use = USED;
	return 0; 
}

void print_mem_list(FILE *fp) {
	curr_mem_chunk = head_mem_list;
	int count = -1;
	while(curr_mem_chunk != NULL) {
		count++;
		fprintf(fp, "\tmem_chunk %d is at %p \n", count, curr_mem_chunk);
		fprintf(fp, "\tIt points to %p \n", curr_mem_chunk->address);		
		curr_mem_chunk = curr_mem_chunk->next;
	}
}

void block_SIGUSR1(sigset_t *sigs, sigset_t *old_sigs) {
    if(sigprocmask(SIG_BLOCK, sigs, old_sigs) != 0) {
    	perror("sigprocmask create block\n");
    	exit(1);
    }
    //printf("Will now block SIGUSR1 signals\n");
}

void rm_block_SIGUSR1(sigset_t *old_sigs) {
	if(sigprocmask(SIG_SETMASK, old_sigs, NULL) != 0) {
    	perror("sigprocmask remove block\n");
    	exit(1);
    }
    //printf("Will now remove block for SIGUSR1 signals\n");
}