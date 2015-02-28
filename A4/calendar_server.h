#ifndef CALENDAR_SERVER_H
    #define CALENDAR_SERVER_H
#endif

#define INPUT_BUFFER_SIZE 256
#define INPUT_ARG_MAX_NUM 8
#define DELIM " \n"

struct client {
  char *name;
  char *buff;
  int fd;
  struct in_addr ipaddr;
  struct client *next;
};

typedef struct client Client;

time_t convert_time(int local_argc, char** local_argv);
int process_args(int cmd_argc, char **cmd_argv, Calendar **calendar_list_ptr, 
                 User **user_list_ptr, Client *p);
int run_calendar(char input[], Client *p);

void bindandlisten();
void newconnection();
Client *addclient(int fd, struct in_addr addr);
void disconnect(Client *p);
static void removeclient(int fd);
static void broadcast_subscription(char *cal_name, char *client_name);
char *read_client_input(Client *p);
