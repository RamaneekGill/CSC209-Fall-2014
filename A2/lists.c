#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lists.h"

Event *extract_earliest_event(Event **event_list_head);
void printout(Event *event_node);

/*                            
 * Return a pointer to the struct calendar with name cal_name                           
 * or NULL if no calendar with this name exists in the cal_list                         
 */                           
Calendar *find_calendar(Calendar *cal_list, char *cal_name) {
	while (cal_list != NULL) {
		if(strncmp(cal_list->name, cal_name, 256) == 0) {
			return cal_list;
		}
		if(cal_list->next == NULL) {
			break;
		}
		cal_list = cal_list->next;
	}
    return NULL;              
}
 
 
/*                            
 * Return a pointer to the user with name username or NULL 
 *   if no such user exists in user_list                   
 */                           
User *find_user(User *user_list, char *username) {
	while (user_list != NULL) {
		if(strcmp(user_list->name, username) == 0) {
			return user_list;
		}
		if(user_list->next == NULL) {
			break;
		}
		user_list = user_list->next;
	}
    return NULL;              
}
 
 
/*                            
 * If a calendar with name cal_name does not exist, create a new                        
 * calendar by this name and insert it at the front of the calendar list.               
 * Return values:             
 *    0 if successful         
 *   -1 if a calendar by cal_name already exists           
 */                           
int add_calendar(Calendar **cal_list_ptr, char *cal_name) {
    if(find_calendar(*cal_list_ptr, cal_name) != NULL) {
		return -1;
	}

	Calendar * new_calendar;

	if ((new_calendar = malloc(sizeof(Calendar))) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	if ((new_calendar->name = malloc(strlen(cal_name) + 1)) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	strcpy(new_calendar->name, cal_name);
 	new_calendar->events = NULL;
 	new_calendar->next = *cal_list_ptr; 
 	
 	*cal_list_ptr = new_calendar;  // Update the head of the list

    return 0;     
}
 
/*                            
 * Print to stdout a list of all current calendar names (one per line)                  
 */                           
void list_calendars(Calendar *cal_list) {
	while(cal_list != NULL) {
		printf("%s\n", cal_list->name);
		cal_list = cal_list->next;
	} 
}

 
/*                            
 * If a user with name username does not exist, create a new                            
 * user by this name and insert it at the end of the user list.                         
 * Return values:             
 *    0 if successful         
 *   -1 if username already exists                         
 */                           
int add_user(User **user_list_ptr, char *username) {

    if(find_user(*user_list_ptr, username) != NULL) {
    	return -1;
    }

    User * user_list_copy = *user_list_ptr; //will be used to point to the current User
    User * new_user;

	if ((new_user = malloc(sizeof(User))) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	if ((new_user->name = malloc(strlen(username) + 1)) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	strcpy(new_user->name, username);
	new_user->subscriptions = NULL;
 	new_user->next = NULL;

 	if(*user_list_ptr == NULL) {  //this is the first user
 		*user_list_ptr = new_user; //create the head of the list with the new user
    	return 0;
 	}

 	while(user_list_copy->next != NULL){ //iterating to the end of list
 		user_list_copy = user_list_copy->next;

 	}

 	user_list_copy->next = new_user;
    return 0;                 
}
 
 
/*                            
 * Print to stdout a list of usernames one per line        
 */                           
void list_users(User *user_list) {
	while(user_list != NULL) {
		printf("%s\n", user_list->name);
		user_list = user_list->next;
	} 
}
 
 
/*                            
 * Subscribe the user username to calendar cal_name        
 * Return:                    
 *    0 if successful         
 *   -1 if no user exists by this name                     
 *   -2 if no calendar exists by this name                 
 *   -3 if this user is already subscribed to this calendar
 */                           
int subscribe(User *user_list, Calendar *cal_list, char *username, char *cal_name) { 
	User * user_subbing = find_user(user_list, username);
	if(user_subbing == NULL) {
    	return -1;
    }

    Calendar * cal = find_calendar(cal_list, cal_name);
	if(cal == NULL){
		return -2;
	}

	Subscription * users_subs = user_subbing->subscriptions;

	//malloc for a new subscription node
	Subscription * new_sub;
	if ((new_sub = malloc(sizeof(Subscription))) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}    

 	new_sub->calendar = cal;
 	new_sub->next = NULL;


 	if (users_subs == NULL) { //if this is the first sub for the user
 		user_subbing->subscriptions = new_sub;
 		return 0;
 	}

	while(users_subs != NULL && users_subs->next != NULL) {
	 	//iterate to end of subscription list
		if(strcmp(users_subs->calendar->name, cal_name) == 0) {
			return -3; //the user is already subbed to this calendar
		}
		users_subs = users_subs->next;
	}

	users_subs->next = new_sub; //from prev while loop:  users_subs->next is NULL
	return 0;
}
 
 
/*                            
 * Add an event with this name and date to the calendar with name cal_name              
 *  Return:                   
 *   0 for success            
 *  -1 for calendar does not exist by this name            
 */                           
int add_event(Calendar *cal_list, char *cal_name, char *event_name, time_t time) {      
    Calendar * cal = find_calendar(cal_list, cal_name);

    if(cal == NULL) {
    	return -1;
    }

    Event * new_event;
    if ((new_event = malloc(sizeof(Event))) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	if ((new_event->description = malloc(strlen(event_name) + 1)) == NULL) { 
 		perror("malloc"); 
 		exit (1);
 	}

 	new_event->time = time;
    strcpy(new_event->description, event_name);
    new_event->next = NULL;

    if(cal->events == NULL) { 	//Calendar doesn't have any events
    	cal->events = new_event; //make new event head of event list
   		return 0;
    } 

	Event * event_copy = cal->events; //will be used to point to the current Event
	Event * prev_event = event_copy; //will be used to point to the previous Event



	//double difftime(time_t time1, time_t time2) returns time2 - time1
	while(prev_event->next != NULL &&
			difftime(event_copy->time, new_event->time) < 0) {
		//while the new event's time is later than the current event's time
		prev_event = event_copy;
		event_copy = event_copy->next;
	}

	//BASE CASES: new event is earlier than first event
	if(difftime(prev_event->time, new_event->time) >= 0) {
		//new event is earliest and should be the head of the event list
		new_event->next = prev_event;
		cal->events = new_event; //update the head of the event list
	}
	//check if to insert inbetween
	else if(event_copy != prev_event) { //if list size is not == 1
		new_event->next = event_copy; //insert inbetween prev and curr event
		prev_event->next = new_event;		
	}

	else {
		prev_event->next = new_event;
	}

    return 0;                 
}
 
 
/*                            
 * Print to stdout a list of the events in this calendar ordered by time                
 *  Return:                   
 *     0 if successful        
 *    -1 if no calendar exists by this name                
 */                           
int list_events(Calendar *cal_list, char *cal_name) {
	Calendar * cal;
	cal = find_calendar(cal_list, cal_name);

    if(cal == NULL) {
    	return -1;
    }

	Event * event_list_copy = cal->events;
	while(event_list_copy != NULL) {
		printf("%s\n", event_list_copy->description);
		event_list_copy = event_list_copy->next;
	}   	

    return 0;                 
}
 
 
/*                            
 * Print to stdout, the description and time of the next event for                      
 * user user_name             
 *  Return:                   
 *   0 on success             
 *   -1 if this user_name does not exist or has no events  
 */
int print_next_event(Calendar *cal_list, User * user_list, char *user_name) {
	User * user = find_user(user_list, user_name);
	if(user == NULL || user->subscriptions == NULL) {
    	return -1;
    }
    
    //Going to be searching for the earliest event in all of the calendars the user
    //is subscribed to, the base case for the earliest event is already done

	Subscription * subs = user->subscriptions;
    Event * events;

	char * next_event_desc = NULL;
	time_t next_event_time;

    while (subs != NULL) { //in each calendar
		events = subs->calendar->events;

    	while (events != NULL) { //each event
    		if(next_event_desc == NULL) { //first event to be compared (the base case)
    				next_event_desc = events->description;
    				next_event_time = events->time;
    			}

    		if(difftime(next_event_time, events->time) > 0) { //this specific event is earlier than the previous ones
    			next_event_desc = events->description;
    			next_event_time = events->time;
    		}
    		events = events->next;
    	}
    	subs = subs->next;
    }

    if(next_event_desc == NULL) { //none of the calendars subscribed to had events
    	return -1;
    }

    struct tm *t;
    t = gmtime(&next_event_time);
    //remember to subtract hour by 4 and add 1 to month for readability
   	printf("%s %d:%.2d-%.2d-%.2d\n", next_event_desc, t->tm_hour - 4, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);

	return 0;                 
}
 
 
/*                            
 * Print to stdout a time-ordered list of all events in any calendars                   
 *  subscribed to by this user
 *                            
 *  Do not change the calendar data-structure.             
 *  Return -1 if this user doesn't exist.                  
 */                           
int print_all_events(Calendar *cal_list, User * user_list, char *user_name) {
	User * user = find_user(user_list, user_name);
	if(user == NULL) {
    	return -1;
    }

    Subscription * subs = user->subscriptions;
    Event * all_events;  //for
    Event *event_list_head = NULL;
    Event *prev_event = NULL;

    //create a linked list of events, copy events in to this list
    //sort list of events by date, then print them, then free them

    while (subs != NULL) { //in each calendar
		all_events = subs->calendar->events;
		

    	while (all_events != NULL) { //each event
			Event * new_event;
		    if ((new_event = malloc(sizeof(Event))) == NULL) { 
		 		perror("malloc"); 
		 		exit (1);
		 	}

		 	if ((new_event->description = malloc(strlen(all_events->description) + 1)) == NULL) { 
		 		perror("malloc"); 
		 		exit (1);
		 	}

		 	strcpy(new_event->description, all_events->description);
		 	new_event->time = all_events->time;
		 	new_event->next = NULL;

		 	if(event_list_head == NULL) { //first duplicate node being created
		 		event_list_head = new_event;
		 	} else { //if there is more than one node now
		 		prev_event->next = new_event;
		 	}

			prev_event = new_event;
    		all_events = all_events->next;
    	}
    	subs = subs->next;
    }    //now a new list of all the events has been created, remember it is unsorted

    Event * printing_event;

	while (1) {
		printing_event = extract_earliest_event(&event_list_head);
		
		if(printing_event == NULL) { //if return value is false
			break;
		} 
		printout(printing_event);  //will also free the duplicate nodes as it prints
	}

    return 0;                 
}
/*
Removes the earliest event from a list of events and returns it as an Event.
*/
Event *extract_earliest_event(Event **event_list_head_ptr) {
	Event * event_list_head = *event_list_head_ptr;
	Event *earliest_event = NULL;
	Event *prev_event = NULL;
	Event *list_copy = event_list_head;
	
	//printf("Extracting an event\n");

	if(event_list_head == NULL) { //if the list is now empty
		return NULL;
	}

	//PROBLEM GOING ON HERE, DOESNT MAKE EVENTLISTHEAD TO NULL SO ABOVE IF NEVER HAPPENS 
	if(event_list_head->next == NULL) { //if there is only 1 node in the list
		//printf("Making event_list_head = NULL since we removed all elements\n");
		*event_list_head_ptr = NULL; //make list empty
		return list_copy;
	}

	//find min event, the list will be of size 2 or more if it gets here.
	while(list_copy != NULL) {
		if(event_list_head != NULL && earliest_event == NULL) { //if list isn't empty and this is first element to process
			earliest_event = event_list_head;
		}
		if(difftime(list_copy->time, earliest_event->time) < 0) {
			earliest_event = list_copy;
		}
		list_copy = list_copy->next;
	}

	//remove earliest event
	//base case: first elem needs to be removed, update head list as firstelem->next
	//other case: keep track of prevelem, prevelem->next = prevelem->currelem->next

	list_copy = event_list_head; //reset list_copy to point to head of unsorted list again

	if(list_copy == earliest_event) { //if the first node of list is the minimum one
		*event_list_head_ptr = list_copy->next; //update head list ptr
		return earliest_event;
	}

	prev_event = list_copy;
	list_copy = list_copy;

	while(list_copy != earliest_event && list_copy != NULL) {
		prev_event = list_copy;
		list_copy = list_copy->next;
	} //now list_copy is the node we want to remove

	prev_event->next = prev_event->next->next; //list_copy is now 'removed' we will free it after printing it

	return earliest_event;
}


/*
Print to stdout an event and frees it's memory after printing, this
function should only operate on the duplicate nodes created from print_all_events
*/
void printout(Event *event_node) {
	struct tm *t;
    t = gmtime(&event_node->time);
    //remember to subtract hour by 4 and add 1 to month for readability
   	printf("%s %d:%.2d-%.2d-%.2d\n", event_node->description, t->tm_hour - 4, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
   	
    //printf("%s %s", event_node->description, ctime(event_node->time));

   	//free the duplicate nodes
   	free(event_node->description);
   	free(event_node); //free the memory that was allocated for the description and the node itself
}

