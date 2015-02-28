Assignment 2: A Simple Calendar Program

Due: 5 PM on Wednesday October 15, 2014

Introduction

Shared electronic calendar tools such as iCal or Google Calendar allow the creation of named calendars each containing individual events. Users then subscribe to multiple calendars each of which may be shared by more than one user.

For this assignment, you will implement a simple online calendar which provides this very basic functionality. Real calendar tools have many fancier features such as repeating events, alarms and graphical user interfaces. You are not expected to provide these.

Data structures

The header file lists.h defines the data structures you must use to build your calendar program. Spend some time examining the code and the figure below and make sure that you understand the structure you are trying to build before you begin to code. Ask questions at office hours or in the help centre or on Piazza.

Your program will have one linked list of calendar objects, each of which contains its own list of events. An event belongs to only one calendar. A single linked list of users contains a node for each user. This node contains the user's name and a pointer to the list of subscriptions for that user. A subscription is simply a pointer to the appropriate calendar in the calendar list. Users can subscribe to any number of calendars and two users can subscribe to the same calendar.

Linked data structures for 
representing calendars, users and subscriptions.
The calendars are stored in reverse order of their creation. Newest caledars are stored at the front of the list. Within a calendar's event list, events are stored in order by date and time with earliest events first. Users are kept in any order, but names must be unique. The code you implement will not allow the addition of a user with the same name as one already in the list. Subscriptions are kept in the order in which they were added. Newest subscriptions are at the tail of the list.

Calendar Operations

The program itself is divided into two main components: a basic calendar API that provides functions to manipulate the calendar data-structures and a user-interface that parses command line instructions and calls the appropriate functions from the API. We are providing the user-interface code in the file calendar.c, which you are not allowed to change. Read it carefully to understand how it works.

Your calendar will be manipulated through a boring (but simple) command line interface that supports the following operations.
Command	Description
add_calendar cal_name	Adds a new empty calendar by this name to the data-structure. The calendar name must be unique.
list_calendars	Lists the names of all the calendars.
add_event cal_name event_name hour [day] [month] [year]	Adds an event to this calendar. The hour must be provided but the optional day, month and year values default to the current date.
list_events cal_name	Prints the complete list of events for this calendar. For each event, prints the description, and the date/time in human readable form.
add_user username	Adds a new user who is not yet subscribed to anything. Username must be unique.
list_users	Lists the names of all the users.
subscribe username cal_name	Subscribes this user to this calendar. Both user and calendar must already exist and this user must not be already subscribed to this calendar.
next_event username	Prints the description and time (again in human readable format) for the next event to which this user is subscribed.
print_all_events username	Prints a list of all the events subscribed to by this user, sorted by time.
quit	Closes the calendar program.
The calendar executable can be started from the command line with either zero or one argument as follows:

 ./calendar [filename] 
If calendar is run without any arguments, it starts in interactive mode where it displays a prompt and waits for one of the above commands from standard input. When run with one argument, calendar expects this argument to be the name of a file that contains one calendar command per line. The commands are those from the list above. The program will execute those commands from the file.

Starter code

The first step of this assignment is to login to MarkUs and navigate to a2. Confirm that your MarkUs repository for a2 is the same as the repository you used for a1. If it is, then update your a1 repo (svn update) and you should now have a directory named a2. Next you need to copy the starter code from ~csc209h/fall/pub/a2-starter into the a2 directory of your repository. Add and commit the files and then log into MarkUs to make sure that you did this properly. If MarkUs has created a new repository for you for a2, (i.e. the path is different than your A1 repo), check out the new a2 repository and add the starter code there.

The starter code contains a Makefile and three files called calendar.c, lists.h, and lists.c that provide the implementation of a skeleton of the calendar data structures. calendar.c provides the code to read and parse the user commands described above, and then calls the corresponding C function for each user command. Read the starter code carefully to familiarize yourself with how it works.

Function prototypes for all functions implementing the various user commands are provided in lists.h. So that the starter code compiles without errors, these functions are currently implemented in lists.c as stubs (i.e. functions whose body is empty except for a return statement), In addition, there are prototypes and stub implementations for a number of helper functions that you will use to implement the user command functions. Your task will be to complete the implementation of all these functions.

We have also provided a sample input file batch_commands.txt that contains a series of calendar commands that will execute in batch mode.

Your tasks

You will complete the functions defined in lists.c. The comment above each function stub further explains the behaviour of the function and sometimes gives some implementation tips.

Note that you are not allowed to modify calendar.c or lists.h. The only C file you are modifying is lists.c. You may add new functions to lists.c, but do not change the names or prototypes of the functions we have given you.

Part I: Managing Calendars (roughly 30%)

Start by implementing and testing the functions that involve only calendars and events. That will mean completing the following functions.

Calendar *find_calendar(Calendar *cal_list, const char *cal_name);
int add_calendar(Calendar **cal_list_ptr, const char *cal_name);
void list_calendars(Calendar *cal_list);
int add_event(Calendar *cal_list, const char *cal_name, const char *event_name, const time_t date);
int list_events(Calendar *cal_list, const char *cal_name);
To print an event, print its description, followed by a colon, then one space and then a human-readable representation of the date and time of the event in the format, "Wed Oct 15 17:00:00 2014". Your textbook has a nice discussion of how time is represented in C that will help you understand the type time_t.

Part II: Adding users (roughly 30%)

Next, write the code for the functions that are called for the user commands add_user, list_users and subscribe.

User *find_user(User *user_list, const char *user_name);
int add_user(User **user_list_ptr, const char *user_name);
void list_users(User *user_list);
int subscribe(User *user_list, Calendar *cal_list, const char *user_name, const char *cal_name);
Part III: Finding Events for Users (roughly 40%)

Finally, implement the two user commands that print events for a given user, starting with the command next_event.

The last command to implement, print_all_events, is a bit tricky but at this point in your CS career, you should be ready for the challenge. You may not assume an upperlimit on the number of calendars to which a user can be subscribed, nor may you assume a limit on the number of events on a given calendar. You also must not destroy the calendar data structure as part of calling this function. I recommend that before you start to code, you use a diagram to sketch out your solution and convince yourself that it will work. Hint: Although not the only solution, one viable approach is to create a copy of each subscribed event and insert it into a sorted list. If you follow this approach, be sure to free the memory you allocate for the copies when you are finished with them. A helper function might be appropriate here and is certainly allowed.

Error handling

The comments for the functions you are to implement define nearly all the error conditions you might encounter and tell you how to handle them. The main additional expectation is that you check the return value from malloc and terminate the program if malloc fails.

What to hand in

Commit to your repository in the a2 directory your revised lists.c file. Since you are not allowed to change lists.h, calendar.c and the provided Makefile, we will use the original versions for compiling and testing your code. The markers must be able to run make such that calendar is compiled with no warnings.

Coding style and comments are just as important in CSC209 as they were in previous courses. Use good variable names, appropriate functions, descriptive comments, and blank lines. Remember that someone needs to read your code. You may write extra helper functions.

Please remember that if you submit code that does not compile, it will receive a grade of 0. The best way to avoid this potential problem is to write your code incrementally. The starter code compiles without warnings and runs. As you solve each small piece of the problem make sure that your new version continues to compile without warnings and run. Get a small piece working, commit it, and then move on to the next piece. This is a much better approach than writing a whole bunch of code and then spending a lot of time debugging it step by step.

