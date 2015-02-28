Assignment A4: Network Programming

Due: Electronically, by 6:00 PM on December 1st

Introduction

In this assignment, you'll be working with Unix sockets to build a calendar server to which clients can connect and issue calendar commands. You will not write a client.

Your Ports

To avoid port conflicts, please use the same port numbers as you used for lab 8. You should also make sure to add the following lines to your server code so that the port will be released as soon as your server process terminates.

    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if(status == -1) {
        perror("setsockopt -- REUSEADDR");
    }
Playing Around

Before detailing your tasks, let's explore the calendar server to get an idea of how it works. On redwolf.cdf.toronto.edu port 8888, I have a calendar server running. To connect to it, type the following at the shell: nc redwolf.cdf.toronto.edu 8888. (You must do this from cdf; only the cdf machines are allowed to connect to this server.) This runs a program callednc to connect to the calendar server.

Once you've connected, you'll be asked for your name. Type a name and hit enter.

Now the server is ready to accept calendar commands:

Try running some of the commands from assignment two to see the behaviour of the server. You will notice that just giving your name doesn't add that name as a user of the calendar system. You still need to call add_user if you want to put to do that. Go ahead and do that. After you have run a few commands, open up another terminal and connect to the calendar server from there too. Choose a different user name and then from that name create a calendar and add the first user name (from your earlier terminal) to a calendar. Notice how the calendar server, notifies the first user that he or she was added to a calendar. If you open yet another terminal and connect another session with the same user name, then both clients will be notified when that name is subcribed to another calendar.

Assuming you still have two or three clients connected, mess around with calendar commands, understanding that the actions of one client (adding an event) are seen when others list events for example. Start typing a command in client 1, then switch to client 2 and type a command: you'll notice that the server responds to client 2; it uses select to avoid waiting for any particular client, and uses separate buffers for each client so that client text is maintained independently.

When you're finished, use ctrl+c to kill your nc process. The quit command will tell the server to disconnect the client, but it won't close your nc process.

Because the calendar interface we are using, only allows the addition of elements (not their removal), I will periodically restart the server to prevent the lists of calendars, users and events from becoming ridiculously long. If your nc session unexpectedly disconnects from the server, it might be because I'm restarting. Don't panic, just start nc again.

Functionality

Your task is to implement the calendar server, as outlined above and further detailed below. You must support the same functionality offered by the sample implementation that you just experimented with.

Login

When a new client arrives, add them to the end of the active clients list, and ask for and store their name.

Note: the server keeps all of its data about clients, calendars and users in memory. Once the server is killed, all user, group, and client information is gone.

Supported calendar Commands

Here are the calendar commands that your calendar server must support. You are encouraged to begin with the solution to A2 calendar provided in your A4 repositories. Here, however, it will be more useful to have functions allocate and return strings of text, rather than use printf to print information locally as in A2 (more of this below).

Command	Description
add_calendar cal_name	Adds a new empty calendar by this name to the data-structure. The calendar name must be unique.
list_calendars	Lists the names of all the calendars.
add_event cal_name event_name hour [day] [month] [year]	Adds an event to this calendar. The hour must be provided but the optional day, month and year values default to the current date.
list_events cal_name	Prints the complete list of events for this calendar. For each event, prints the description, and the date/time in human readable form.
add_user username	Adds a new user who is not yet subscribed to anything. Username must be unique.
list_users	Lists the names of all the users.
subscribe username cal_name	Subscribes this user to this calendar. Both user and calendar must already exist and this user must not be already subscribed to this calendar.
quit	Closes the client connection.
When a user calls subscribe username cal_name, not only is username subscribed to cal_name, but any clients currently connected who have that same username are notified of the subscription. Clients with different usernames do not receive the notification message.

Other calendar commands from A2 are not required here and we have removed the functionality from the starter code.

Your tasks

Step 1:

Modify the list_users, list_calendars, and list_events functions to return dynamically allocated strings in all cases.

Notice that this list (in the original handout posted) used to contain add_users, add_calendar, and subscribe If you already made changes for these functions, you can leave them except for subscribe. You want to have it still return an integer, so that you can notify logged-in users with the same username. You could also change add_event if you wish.

When the user types a command in the client, it will be sent to the server. The server will call the appropriate function, and will send a message back to the client to be printed on stdout to the user. This will be easier to do if the functions in lists.c return strings instead of printing to stderr or stdout. In A2, some of the functions returned integers indicating success or failure and then the calling code in calendar.c generated the error message. Other than for subscribe you may switch this so that the functions in list.c return the messages in the first place if you wish.

If you look at the code for add_user, you will notice that it also might print an error message if malloc fails. This can still be printed to stderr and the server can exit, since there is not much you can do if you can no longer allocate memory.

Some commands (list_users and list_calendars) require a string to be created based on the nodes in a linked list. However, prior to looping through the list, you don't know how much memory to allocate for the string that you will build. If you are concatenating strings together, then one approach is to loop through the list twice: once to add up all of the string lengths, and again to copy those strings into a buffer of sufficient size. You may find snprintf helpful.

Because you are writing a server that can be accessed by malicious users across the internet, it is a requirement of the assignment that your code never overwrite the end of a character array. This means that when you use malloc to allocate memory for strings you guarantee to allocate enough memory to hold the strings to be stored. Since the user will be typing arbitrarily sized user names, calendar names and event names, you will often have to check the size of the input before allocating memory. Inside your function list_users write an extra comment that explains why you are guaranteed not to overwrite the space you have allocated for the return value. This explanation may be longer than a comment that you would typically include in a program. We will be marking this explanation and it is helpful to have it right next to the code to which it refers.

Make sure to modify the calendar.c starter code to so that it works again after you have changed all of these functions. For example, instead of

    if (strcmp(cmd_argv[0], "add_calendar") == 0 && cmd_argc == 2) {
        if (add_calendar(calendar_list_ptr, cmd_argv[1]) == -1) {
            error("Calendar by this name already exists");
        }
you might have something like

    if (strcmp(cmd_argv[0], "add_calendar") == 0 && cmd_argc == 2) {
        buf = add_calendar(calendar_list_ptr, cmd_argv[1]);
		// The next print statement will eventually be replace by writing to the client.
        printf("%s",buf);
        ...
Be sure that your changes to lists.c work properly before you move on to the next step!
Step 2: Build the server so that it works with one client

Add the socket code so that you can have one client writing calendar commands to the server. Take advantage of the example code you have been given.

Partial reads

Calling read on a socket is not guaranteed to return a full line typed in by the client, so you will need to keep track of how much is read each time and check to see if you have received a newline which indicates the end of a command. (Section 61.1 in Kerrisk describes this in detail.) Lab 8 should also be helpful here.

Step 3: Support multiple clients using select

The server must never block waiting for input from a particular client or the listening socket. (After all, it can't know whether a client will talk next or whether a new client will connect. And, in the former case, it can't know which client will talk next.) This means that you must use select rather than blocking on one file descriptor. (There will be a deduction if you code could block on a read or accept call.)

Note that you will need some kind of data structure to keep track of clients, including a buffer to hold partial reads.

Step 4 (Step 0): Makefile

This should really be Step 0, since you will save yourself time if you create a Makefile right from the start and use it as your are developing your solution.

Create a Makefile that compiles a program called calserver. In addition to building your code, your Makefile must permit choosing a port at compile-time. To do this, first add a #define to your program to define the port number on which the server will expect connections (this is the port based on your student number):

#ifndef PORT
  #define PORT <x>
#endif
Then, in your Makefile, include the following code, where y should be set to your student number port plus 1:
PORT=<y>
CFLAGS= -DPORT=\$(PORT) -g -Wall
Now, if you type make PORT=53456 the program will be compiled with PORT defined as 53456. If you type just make, PORT will be set to y as defined in the makefile. Finally, if you use gcc directly and do not use -D to supply a port number, it will still have the x value from your source code file. This method of setting a port value will make it possible for us to test your code by compiling with our desired port number. (It's also useful for you to know how to use -D to define macros at commandline.)

Sample Code

Here is a sample server written by Alan Rosenthal that you might find helpful. Feel free to yank code from there, with two important provisos:

Don't copy-and-paste stuff into your program and then fuss with it to make it work. You should know exactly what the code does and why. We won't take kindly to extra stuff in your code that is unnecessary or does not work, and is clearly left over from the sample server.
Clearly indicate the code that you took from the sample server.
Testing

Since you're not writing a client program, the nc tool mentioned above can be used to connect clients to your server. Your server is required to work in noncanonical mode (stty -icanon); we will test in this mode. For completeness, you might also wish to test in canonical mode (stty icanon). We won't explicitly be testing in this mode, but problems here might indicate unfounded assumptions in your server.

To use nc, type nc hostname yyyyy, where hostname is the full cdf name of the machine on which your server is running, and yyyyy is the port on which your server is listening. If you aren't sure which machine your server is running on you can run hostname -f to find out. If you are sure that the server and client are both on the same server, you can use localhost in place of the fully specified host name.

To test if your partial reads work correctly, you can set the terminal to send characters to the socket as soon as you type it, by running stty -icanon. When you want to go back to the normal line buffered mode, type stty icanon at the shell to return the terminal back to canonical mode. If you don't do this, programs will work in unexpected ways. (For example, do a stty -icanon to get back to noncanonical mode, then type cat. You'll notice that instead of waiting for full lines, each character is echoed by cat as soon as you type it.) When you are using non-canonical mode, characters get sent immediately to the server, so it is more apparent that the server is waiting to see a newline character bfore it processes the input.

What to submit

Commit to the a4 directory in your repository all .h and .c files, and your Makefile.

Remember to be sure that your program compiles without warning when you type make and that it produces an executable called calserver.

Make sure that you've done svn add on every file. As a final step, checkout your repository into a new directory to verify that everything is there. There is nothing we can do if you forget to submit a file.