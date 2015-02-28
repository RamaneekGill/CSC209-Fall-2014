#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include "lists.h"
#include "calendar_server.h"


//TODO
  //compiling
  //testing

#ifndef PORT
  #define PORT 55754 
#endif

//Global Vars
FILE *output_file;
//Create the heads of the two empty data-structures 
Calendar *calendar_list = NULL;
User *user_list = NULL;

int num_clients = 0; //for coolness factor when connected to the server
static int listenfd;

Client *top = NULL;

int main(void) {
  Client *p;

  bindandlisten();

  while(1) {
    fd_set fdlist;
    int maxfd = listenfd;
    FD_ZERO(&fdlist);
    FD_SET(listenfd, &fdlist);  //add listener to master fdset

    for (p = top; p; p = p->next) { //each client's listener is set on in master fdset
      FD_SET(p->fd, &fdlist);
      if (p->fd > maxfd)  //if this client's fd_set is bigger than the master fd_set update it
        maxfd = p->fd;
    }

    if (select(maxfd + 1, &fdlist, NULL, NULL, NULL) < 0) {
      perror("select");
      exit(1);
    }

    for (p = top; p; p = p->next) {//lets go through each client and see what they're up to
      
      if (FD_ISSET(p->fd, &fdlist)) {//this client is now ready to read
        //printf("ready to read from %s has buf of %s\n", p->name, p->buff);
        //printf("Will now read from %s \tbuffer leftover is |%s|\n", p->name, p->buff);
        char *calendar_command = read_client_input(p);

        if(calendar_command != NULL) {
          //will run calendar and write to it accordingly
          //printf("Calendar command is %s \n", calendar_command);
          run_calendar(calendar_command, p);
        }

      }  //end of reading
    }  //end of for

    if (FD_ISSET(listenfd, &fdlist))
      newconnection();

  } //end of while

  return 0;
} //end main


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////             METHODS            //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


/* 
 * Return a calendar time representation of the time specified
 *  in local_args. 
 *    local_argv[0] must contain hour. 
 *    local_argv[1] may contain day, otherwise use current day
 *    local_argv[2] may contain month, otherwise use current month
 *    local_argv[3] may contain day, otherwise use current year
 */
time_t convert_time(int local_argc, char** local_argv) {

  time_t rawtime;
  struct tm * info;  

  // Instead of expicitly setting the time, use the current time and then
  // change some parts of it.
  time(&rawtime);            // get the time_t represention of the current time 
  info = localtime(&rawtime);   

  // The user must set the hour.
  info->tm_hour = atof(local_argv[0]);

  // We don't want the current minute or second. 
  info->tm_min = 0;
  info->tm_sec = 0;

  if (local_argc > 1) {
     info->tm_mday = atof(local_argv[1]);
  }
  if (local_argc > 2) {
     // months are represented counting from 0 but most people count from 1
     info->tm_mon = atof(local_argv[2])-1;
  }
  if (local_argc > 3) {
     // tm_year is the number of years after the epoch but users enter the year AD
     info->tm_year = atof(local_argv[3])-1900;
  }

  // start off by assuming we won't be in daylight savings time
  info->tm_isdst = 0;
  mktime(info);
  // need to correct if we guessed daylight savings time incorrect since
  // mktime changes info as well as returning a value. Check it out in gdb.
  if (info->tm_isdst == 1) {
     // this means we guessed wrong about daylight savings time so correct the hour
     info->tm_hour--;
  }
  return  mktime(info);
}


/* 
 * Read and process calendar commands
 * and writes to the client calendar's output and error
    There are memsets all over to make sure garbage isnt printed to client
    THIS IS TESTED AND NEEDED FOR WEIRD SHITTY CORNER CASES
 */
int process_args(int cmd_argc, char **cmd_argv, Calendar **calendar_list_ptr, 
                 User **user_list_ptr, Client *p) {

    Calendar *calendar_list = *calendar_list_ptr; 
    User *user_list = *user_list_ptr;
    char client_output[INPUT_BUFFER_SIZE];

    if (cmd_argc <= 0) {  //why did she include this?, it'll never happen
        return 0;
    } else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
        disconnect(p);
        
    } else if (strcmp(cmd_argv[0], "add_calendar") == 0 && cmd_argc == 2) {
        if (add_calendar(calendar_list_ptr, cmd_argv[1]) == -1) {
            strcat(client_output, "Calendar by this name already exists\n");
            write(p->fd, client_output, strlen(client_output));
        }
        
    } else if (strcmp(cmd_argv[0], "list_calendars") == 0 && cmd_argc == 1) {
        write(p->fd, list_calendars(calendar_list), strlen(list_calendars(calendar_list)));
        
    } else if (strcmp(cmd_argv[0], "add_event") == 0 && cmd_argc >= 4) {
        // Parameters for convert_time are the values in the array
        // cmd_argv but starting at cmd_argv[3]. The first 3 are not
        // part of the time.
        // So subtract these 3 from the count and pass the pointer 
        // to element 3 (where the first is element 0).
        time_t time = convert_time(cmd_argc-3, &cmd_argv[3]);

        if (add_event(calendar_list, cmd_argv[1], cmd_argv[2], time) == -1) {
          strcat(client_output, "Calendar by this name does not exist\n");
          write(p->fd, client_output, strlen(client_output));
        }
//write(p->fd, , strlen()); 
    } else if (strcmp(cmd_argv[0], "list_events") == 0 && cmd_argc == 2) {
        write(p->fd, list_events(calendar_list, cmd_argv[1]), 
              strlen(list_events(calendar_list, cmd_argv[1])));

    } else if (strcmp(cmd_argv[0], "add_user") == 0 && cmd_argc == 2) {
        if (add_user(user_list_ptr, cmd_argv[1]) == -1) {
          strcat(client_output, "User by this name already exists\n");
          write(p->fd, client_output, strlen(client_output));
        }

    } else if (strcmp(cmd_argv[0], "list_users") == 0 && cmd_argc == 1) {
        write(p->fd, list_users(user_list), strlen(list_users(user_list)));
        
    } else if (strcmp(cmd_argv[0], "subscribe") == 0 && cmd_argc == 3) {
        int return_code = subscribe(user_list, calendar_list, cmd_argv[1], cmd_argv[2]);

        if (return_code == 0) {
          broadcast_subscription(cmd_argv[2], p->name);
        } else if (return_code == -1) {
          strcat(client_output, "User by this name does not exist\n");
          write(p->fd, client_output, strlen(client_output));
        } else if (return_code == -2) {
          strcat(client_output, "Calendar by this name does not exist\n");
          write(p->fd, client_output, strlen(client_output));
        } else if (return_code == -3) {
          strcat(client_output, "This user is already subscribed to this calendar\n");
          write(p->fd, client_output, strlen(client_output));
        }
    
    } else {
        strcat(client_output, "Incorrect syntax\n");
        write(p->fd, client_output, strlen(client_output));
    }


    memset(client_output, 0, sizeof(client_output));
    strcat(client_output, ">");
    write(p->fd, client_output, strlen(client_output));
    return 0;
}

/*
  Will take in client's input after it has been validated by read_client_input()
  Will also take in Client and will pass it to process_args who will then
  write to the client the result of the calendar app.
*/
int run_calendar(char input[], Client *p) {
  char *cmd_argv[INPUT_ARG_MAX_NUM];
  int cmd_argc;
  // tokenize arguments
  // Notice that this tokenizing is not sophisticated enough to handle quoted arguments
  // with spaces so calendar names, event descriptions, usernames etc. can not have spaces.
  char *next_token = strtok(input, DELIM);
  cmd_argc = 0;

  while (next_token != NULL) {
    if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
      char client_output[25];
      strcat(client_output, "Too many arguments!\n");
      write(p->fd, client_output, strlen(client_output));
      cmd_argc = 0;
      break;
    }
    cmd_argv[cmd_argc] = next_token;
    cmd_argc++;
    next_token = strtok(NULL, DELIM);
  }
  process_args(cmd_argc, cmd_argv, &calendar_list, &user_list, p);
  //memset(p->buff, 0, sizeof(p->buff)); 

  //so i can read the next cal command properly
  free(p->buff);
  //printf("freeing p->buff and reallocing it\n");
  if((p->buff = malloc(INPUT_BUFFER_SIZE)) == NULL) {
    perror("malloc");
    exit(1);
  }
  return 0;
}

void bindandlisten() {
  struct sockaddr_in r;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      exit(1);
  }

  memset(&r, '\0', sizeof r);
  r.sin_family = AF_INET;
  r.sin_addr.s_addr = INADDR_ANY;
  r.sin_port = htons(PORT);

  if (bind(listenfd, (struct sockaddr *)&r, sizeof(r))) {
      perror("bind");
      exit(1);
  }

  if (listen(listenfd, 10)) { //max 10 connections in queue
      perror("listen");
      exit(1);
  }
}

void newconnection() {
  int fd;
  struct sockaddr_in r;
  socklen_t socklen = sizeof r;

  if ((fd = accept(listenfd, (struct sockaddr *)&r, &socklen)) < 0) {
    perror("accept");
    exit(1);
  } else {
    fprintf(stderr, "connection from %s\n", inet_ntoa(r.sin_addr));
    Client *p = addclient(fd, r.sin_addr);
    char buf[INPUT_BUFFER_SIZE];
    sprintf(buf, "Please enter your username: \n");
    write(p->fd, buf, strlen(buf));
  }
}

Client *addclient(int fd, struct in_addr addr) {
  Client *p = malloc(sizeof(Client));

  if (!p) {
    perror("malloc for client");  /* highly unlikely to happen */
    exit(1);
  }

  if((p->buff = malloc(INPUT_BUFFER_SIZE)) == NULL) {
    perror("malloc");
    exit(1);
  }

  if((p->name = malloc(INPUT_BUFFER_SIZE)) == NULL) {
    perror("malloc");
    exit(1);
  }

  fprintf(stderr, "Adding client %s\n", inet_ntoa(addr));
  p->fd = fd;
  p->ipaddr = addr;
  p->next = top;
  top = p;
  num_clients++;

  //printf("client added now theres %d\n", num_clients);
  return p;
}
/*
  Removes the client from the server cleanly and says goodbye! :) 
*/
void disconnect(Client *p) {
  fprintf(stderr, "Disconnect from %s\n", inet_ntoa(p->ipaddr));
  char buf[80];
  sprintf(buf, "Goodbye %s\n", inet_ntoa(p->ipaddr));
  write(p->fd, buf, strlen(buf));
  removeclient(p->fd);
}

static void removeclient(int fd) {
  Client **p;
  for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next)
      ; //for loop gets the client with the same fd we want to remove
  if (*p) {
    Client *t = (*p)->next;
    fprintf(stderr, "Removing client %s\n", inet_ntoa((*p)->ipaddr));
    free(*p);
    *p = t; //update the removed client's next reference
    num_clients--;
  } else {
    fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
  }
}

/*
  Lets all the same users that are logged in know if they subbed to a calendar 
*/
static void broadcast_subscription(char *cal_name, char *client_name) {
  Client *p;
  //INPUT_BUFFER_SIZE for cal_name, + 50 for the message
  char client_output[INPUT_BUFFER_SIZE + 50];
  sprintf(client_output, "You just subscribed to %s!", client_name);

  for (p = top; p; p = p->next) {
    if(strcmp(p->name, client_name) == 0) {
      write(p->fd, client_output, strlen(client_output));
    }
  }
}



/*
  Will read the client input correctly, creates the client's username first.
  Wait for client to login with a username. Iff logged in waits for calendar command
  and returns ONLY the calendar_command as a string.
*/
char *read_client_input(Client *p) {
  char client_input[INPUT_BUFFER_SIZE];
  int len_read;


  if(strlen(p->name) != 0) { //then we will read calendar commands
    if((len_read = read(p->fd, client_input, sizeof(client_input))) < 0) {
      perror("read");
      exit(1);
    }

    //if we get here that means something was read
    int i;
    for(i = 0; i < strlen(client_input); i++) {
      if(client_input[i] == '\n') {
        p->buff[i] = client_input[i];
        break;
      }
      p->buff[i] = client_input[i];
    }
    //printf("reading, p->buff is |%s|\n", p->buff);

    p->buff[strlen(p->buff)] = '\0';

    //'\n' and '\r\n' for unix and window communication, learned this in office hours
    if (p->buff[strlen(p->buff)-1] == '\n') { //|| p->buff[strlen(p->buff)] == '\r\n') {
      //return the calendar command since it has been fully read.
      return p->buff;
    }

  } 

  else { //still reading for a username, make sure it ends with '\n' pr '\r\n'    
    if((len_read = read(p->fd, client_input, sizeof(client_input))) < 0) {
      perror("read");
      exit(1);
    }

    //if we get here that means something was read
    int i;
    for(i = 0; i < strlen(client_input); i++) {
      if(client_input[i] == '\n') {
        p->buff[i] = client_input[i];
        break;
      }
      p->buff[i] = client_input[i];
    }

    p->buff[strlen(p->buff)] = '\0';

    //'\n' and '\r\n' for unix and window communication, learned this in office hours
    if (p->buff[strlen(p->buff)-1] == '\n') { //|| p->buff[strlen(p->buff)] == '\r\n') {
      strcat(p->name, p->buff);
      memset(p->buff, 0, sizeof(p->buff)); //buffer should be cleared since I read full string
      char buf[INPUT_BUFFER_SIZE];;
      sprintf(buf, "Welcome to Calendar! %s\nPlease type a command:\n>", p->name);
      write(p->fd, buf, strlen(buf));
    }
  }

  return NULL;
}