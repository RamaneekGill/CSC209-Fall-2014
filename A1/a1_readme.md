Assignment A1: Using the Shell and Intro C




Home page Course Info Sheet Piazza (DB) PeerWise Anonymous Feedback 
Course Work Assignments Labs MarkUs Lecture Notes 
Grades Tests Forms

Part 1: Using the Shell (3%)

Introduction

Follow the instructions carefully, so that we receive your work correctly.

Your first step should be to log into MarkUs using your cdf id and password. Select the assignment A1: Using the Shell and Intro C. You will be working alone on this assignment, and the URL for your SVN repository can be found on the right hand side of the page. Use that URL to check out your repository. You will find an a1 directory there that you should put your work in.

Once you have committed files to your repository, you can use MarkUs to make sure that what you committed is what we got. The Submissions tab for the assignment will show you what is in the repository, and will let you know if you got the file names right. You will not be able to submit files through the MarkUs web interface for assignments.

You must do all of the exercises on one of the CDF lab machines. After you perform each task, you will run a program that tells you whether your results for that task are correct, and sends those results to us. There is no penalty for incorrect answers, so you can fix any mistakes you made and try again.

It is important that you follow the instructions carefully, so that your work is correctly recorded. Remember, you must do all of the steps on a CDF machine from your own account. You can do it from home if you are logged into a CDF server (using NX, Putty, ssh).

Commands or programs that you need to do this assignment were mostly covered in class, but there may be a few that weren't. A possibly incomplete list of these commands: ls, cd, chmod, cut, grep, ps, mkdir, touch, export.

1. Check out your SVN repository

Create a directory in your home directory called csc209. Change into that directory and check out your SVN repo using the repository link from MarkUs.

Change into the a1 directory. Use a text editor to create a file inside a1 called results.txt Enter your name and your cdf id as the first line of the file. Save and commit the file. The programs that check your results will be adding to this file, but you shouldn't need to edit it by hand any more.

Run /u/csc209h/fall/pub/bin/checkstep1

Read the output to check to see if the checking program thinks you did the first step correctly. If not, fix it and rerun the program before you move on.

2. Change your shell

If you haven't already changed your shell to bash, do it now using chsh. Note that chsh needs two arguments. The first is your user name, and the second is "bash". The chsh program connects to a password server to make the change. You will need to log out and log back in to see the effect and it may take a few minutes for the change to propagate.

Run /u/csc209h/fall/pub/bin/checkstep2

(If you really want to change it back to tcsh, you can do so after you are finished with this assignment.)

3. Add /u/csc209h/fall/pub/bin to your PATH

It's kind of annoying to have to keep typing in the full path to the projects that check your work for this assignment, so add /u/csc209h/fall/pub/bin to your PATH variable.

Modifying environment variables was not covered in class. When a shell variable is used, it must be prefaced with a $, but when it is defined it does not have the $ at the front. In order to make variables visible to programs running inside the shell (or subshells), use export command. To add the directory /foo to your path you could use the command export PATH=/foo:$PATH. Note that there cannot be any spaces around the "=", and recall that the PATH is a colon-separated list of paths.

Now run checkstep3 (without using the full path to it).

4. Add it permanently

Now add the path from step 3 to your PATH variable so that when you start up a new shell the csc209 directory will be in your PATH. You can do this by adding the command from step 3 to your .bashrc file.

(We're not checking this step.)

5. Environment variables

You have already seen a few standard environment variables. You can also create your own. Create an environment variable called COLOUR with the value of your favourite colour. Use echo to print it to the screen. (Remember that the convention is that environment variables are all upper case.)

Run checkstep5.

6. Files and permissions

Create a directory somewhere in your account where you can create a set of file and subdirectories so that when you run ls -l you will see the following. You will see a different owner, group, dates, and you may see a different file size, but the permissions and the files names should be exactly the same as below.

You can create directories using mkdir and you can create files using your favourite editor or using touch. Read the man pages for these two commands to find out how they work (e.g., man mkdir).

sh-3.2$ ls -l
total 2
drwxr-xr-x 2 mcraig instrs 512 Sep 16 10:07 adir
d--x--x--x 2 mcraig instrs 512 Sep 16 10:04 bdir
-rw-r----- 1 mcraig instrs   0 Sep 16 10:04 file1
-rwxrwxrwx 1 mcraig instrs   0 Sep 16 10:04 file2
-rwx------ 1 mcraig instrs   0 Sep 16 10:05 file3

Copy longproc from /u/csc209h/fall/pub/bin to adir. Now running ls -l * should look like the following:

sh-3.2$ ls -l *
-rw-r----- 1 mcraig instrs   0 Sep 16 10:04 file1
-rwxrwxrwx 1 mcraig instrs   0 Sep 16 10:04 file2
-rwx------ 1 mcraig instrs   0 Sep 16 10:05 file3

adir:
total 9
-rwxr-xr-x 1 mcraig instrs 8344 Sep 16 10:07 longproc
ls: cannot open directory bdir: Permission denied
Make sure you are in the directory where the ls commands will run as shown above. The current working directory must be the one that contains the 3 files and 2 directories as shown above. Now run checkstep6

7. Processes

In the previous step, you copied longproc into adir. Make sure your current working directory is the parent directory of adir. Without changing into the adir directory, run longproc using a relative path. Keep longproc running either in the background, or in another window. Find out what its process id is, and run checkstep7 -p PID where PID is the process id for longproc.

Now, stop the longproc process without terminating it, and run checkstep71 -p PID where PID is the process id of the stopped longproc process.

Now make sure that all your longproc processes have been terminated, and run checkstep72

8. Grep

Run ps aux and save the output to a file called psout. Using grep, another command, and a pipe, find out how many processes in the psout snapshot are not owned by root.

Run checkstep8 -n NUM where NUM is the number of processes not owned by root. (The program will check your answer, but it will also check the time stamp on the psout file and if it is more than 3 minutes old, it will ask you to run the ps command again.)

9. Commit your results

Run svn add and svn commit on your results.txt file.

That's the end

You have reached the end of the tasks for assignment 1, part 1. Remember that you can repeat them and rerun the checkstep programs as often as you like. Because the checkstep programs log the results, you should be able to tell exactly how many steps you have completed successfully.

Part 2: A First C Program (3%)

You will write a C program called largest.c that could be used to inspect the output from the ls -l command and print the name and size of the largest group-readable file.

Details

From standard input, your program reads input in the same format as produced by the ls program with the -l option when run on cdf.

Your program prints to standard output, the name of the largest group-readable regular file and its size separated by one tab and followed by a newline character. Directories are not considered to be regular files.

Your program takes two optional commandline arguments. The first is -g and the second is a group name. If called with this option, your program will only consider the files in the input listing that belong to this specified group.

Assumptions You May Make

This may be your first C program. There are a number of assumptions that would not be reasonable in the real world (or even in later assignments this term.) For this assignment only, you may assume the following:
There is at least one group-readable regular file in the input listing. If the -g option is specified, at least one group-readable regular file is in the specified group.
The user will either give no commandline arguments or two and if the user has specified two commandline arguments, the first is -g.
The input listing contains only regular files and directories. (There are no links or other unusual file types.)
Names of things (filenames, usernames, groupnames, ...) will be at most 31 characters.
An integer is large enough to hold any file sizes for this assignment.
Filenames do not have spaces in them. In general this is not a valid assumption in CSC209 assignments.
Processing the Commandline Arguments

Assumption 2 above is particularly unrealistic for real C programs. We would normally want to have many different options and would design our programs to behave like other C tools where options can be specified in any order and grouped together. In the future, you will learn about the getopt library that makes processing commandline arguments easy. But for now, you are not expected to use getopt. Feel free to read up on it and try it, but there are no marks for this and no expectation that you have time to do this.

Reminder

Your program must compile on cdf using gcc with the -Wall option and should not produce any error messages. Programs that do not compile, will get 0. You can still get part marks by submitting something that doesn't completely work but does some of the job -- but it must at least compile to get any marks at all.

Good Style and All That

As always, you should use good style. That means clear variable names, consistent style, wise use of white-space, helpful comments, use of #DEFINE constants where appropriate, etc. Yes, there will be marks for this.

Submission

We will be looking for the following files in your repository:

results.txt
largest.c