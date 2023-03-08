//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>


/*
Basics for what I want to do:

Complete - Provide a prompt for running commands
Complete - Handle blank lines and comments, which are lines beginning with the # character
Complete - Provide expansion for the variable $$
Complete - Execute 3 commands exit, cd, and status via code built into the shell
Complete - Execute other commands by creating new processes using a function from the exec family of functions
Complete - Support input and output redirection
Support running commands in foreground and background processes
Complete - Implement custom handlers for 2 signals, SIGINT and SIGTSTP

*/

/*
what I need to keep track of:
background or foreground
exit status or regular status
*/

//from lecture 10
/*
struct sigaction {
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int, siginfo_t*, void*);
};
*/

//flags for each process
int foreground_only_mode = 0;
int background = 0;

//status
int status;

//custom for SIGINT and SIGSTP
//taken from Signal Handling API Exploration
//this is basic if we are running only background then print message that's what we're doing
//else print opposite message then flush stdout for housekeeping
void handle_SIGTSTP(int signo) {
    //set if we are exiting
    if (foreground_only_mode) {
        foreground_only_mode = 0;
        background = 0;
        printf("\nExiting foreground-only mode (& is now allowed)\n");
        printf(": ");
    
    //set if we are entering
    } else {
        foreground_only_mode = 1;
        printf("\nEntering foreground-only mode (& is now ignored)\n");
        printf(": ");
    }
}

//defines
#define MAX_ARG 512
#define MAX_IN 2048

//prototypes
char * input(char * inArgs[MAX_ARG]);
void run();
void split(pid_t pid, char * inArgs[MAX_ARG], int count);
void redirectIn(char * inArgs[MAX_ARG], int i);
void redirectOut(char * inArgs[MAX_ARG], int i);

int main() {
    //loop to continuously ask for input
    fflush(stdin);
    fflush(stdout);
    while(1){
        
        //run shell
        fflush(stdin);
        fflush(stdout);
        background = 0;

        run();
    }
    return 0;
}

void run(){

    //get input from user for proccessing
    char * inArgs[MAX_ARG];
    input(inArgs);

    //get count of args
    int count = 0;
    int i = 0;
    for(i; i < MAX_ARG; i++){
        if(inArgs[i] != NULL){
            count++;
        }
        else{
            i=MAX_ARG;
        }
    }

    //scary looking sigint and sigstp stuff
    //signal handler for SIGTSTP

    /* What they do...
        -init SIGSTP_action to all zeroes 
        -make handle_SIG.. to handle incoming signals
        -ignore other signals via sigfillset
        -restart previous stuff with SA_restart
        -the we pass in our functions to our stigacation so it knows what to do (init)
    */
    struct sigaction SIGTSTP_action = {0};
    SIGTSTP_action.sa_handler = handle_SIGTSTP;
    sigfillset(&SIGTSTP_action.sa_mask);
    SIGTSTP_action.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    //ignoring SIGINT signal
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);


    //set background correctly
    if(strcmp(inArgs[count-1], "&") == 0){
        background = 1;
        inArgs[count-1] == NULL;
        count -= 1;
    }

    //three commands that don't need child process
    //status
    if(strcmp(inArgs[0], "status") == 0){
            if (WIFEXITED(status)){
				printf("exit value %d\n", WEXITSTATUS(status));
			}
			else if (WIFSIGNALED(status)){
				printf("terminated by signal %d\n", WTERMSIG(status));
			}
            else{
                printf("unknown termination value or signal %d\n", WTERMSIG(status));
            }
    }

    //exit
    else if(strcmp(inArgs[0], "exit") == 0){
        exit(0);
    }

    //cd - if no second argument go home otherwise change to provided arg
    else if(strcmp(inArgs[0], "cd") == 0){
        //get directory specified by "HOME"
        if(inArgs[1] == NULL){
            chdir(getenv("HOME"));
            char cwd[MAX_IN];
            getcwd(cwd, sizeof(cwd));
        }
        //change to provided arg
        else{
            chdir(inArgs[1]);
            char cwd[MAX_IN];
            getcwd(cwd, sizeof(cwd));
        }
        return;
    }

     //all others split
    else {
        pid_t pid;
        split(pid,inArgs, count);
    }
}

char * input(char * inArgs[MAX_ARG]){
    //input inits
    char inputs[MAX_IN];
    //print : while input is empty
    bool valid = false;

    while(!valid){
        printf(": ");
        //get input and put it in char array
        fgets(inputs, MAX_IN, stdin);
        //parse char array
        strtok(inputs, "\n");
        
        //this check for comments and blank lines
        if(strlen(inputs) >= 1 && inputs[0] != '#' && inputs[0] != '\n'){valid = true;};
    }

    //this makes it easier for us to use it later (also helps us track commands)
    //we can extract args and commands by accounting for spaces
    char * nextArg = strtok(inputs, " ");
    int numIn = 0; 

    while(nextArg != NULL){
        char * arg = nextArg;

        int i = 0;
        int j = 0;

        //this does expansion
       for(i; i < strlen(arg); i++){
             if (arg[i] == '$' && arg[i+1] == '$' && (i + 1 < strlen(arg))) {
                char pid[10];
                char * new=malloc(sizeof(char)*MAX_IN);
                arg[i] = '%';
                arg[i+1] = 'd';
                sprintf(new, arg, getpid());

                arg = new;
                j = i+2;
                i = j;
            } 
        
       }
        inArgs[numIn]= arg;
        numIn ++;
        nextArg = strtok(NULL, " ");
    }

    //set the final val to null
    inArgs[numIn] = NULL;

    fflush(stdin);
    return *inArgs;
}

void split(pid_t pid, char * inArgs[MAX_ARG], int count){
    /* this is the most important function in the shell 
    --This function forks a process
    --It checks if it was successful
    --It loops through the arguments given in the command line
    --check for redirection by matchin a < or a > then opens the next provided arg
    --Uses dup2 as prescribed by assignment doc
    --adds command args to diff array so we don't accidentally get an error where it runs
    the same command as an argument of itself (e.g ls ls)
    --terminates loop
    --calls execvp if not redirection 
    */
    pid = fork();
    int count2 = 0;

    //structure taken from Lecture 3
    switch(pid){
        case -1: 
            perror("Split failure \n");
            exit(1);
            return;
        case 0: ;
            int i = 0;
            int j = 0;

            //run background stuff
            struct sigaction SIGINT_action = {0};
            SIGINT_action.sa_handler = SIG_DFL;
			sigaction(SIGINT, &SIGINT_action, NULL);


            // create a new array to hold the command and its arguments
            char * cmdArgs[MAX_ARG];
            for(i = 0; i < count; i++){
                //input redirection
                if(strcmp(inArgs[i],"<") == 0){
                    redirectIn(inArgs, i);
                    i++;
                } 
                //output redirection
                else if(strcmp(inArgs[i], ">") == 0){
                   redirectOut(inArgs, i);
                   i++;

                } 
                //otherwise not a redirection it's an arg
                else {
                    // add the argument to cmdArgs if not redirection
                    cmdArgs[j] = inArgs[i];
                    j++;
                }
            }

            //end cmdArgs with NULL
            cmdArgs[j] = NULL; 

             //if & at the end we need to disregard and set background
            if(strcmp(cmdArgs[j-1], "&") == 0){
                //set background
                if(foreground_only_mode == false){
                    background = 1;
                    count--;
                    inArgs[count-1] = NULL;
                }

                //this disregards
                inArgs[count-1] = NULL;
                count -=1;
            }

            //execute the command in foreground
            if(background != 1){
                int error = execvp(cmdArgs[0], cmdArgs);
                if(error == -1){
                    perror("execvp");
                    exit(1);
                }
            }

            else{
                    //pid of child
                    pid_t pid2 = getpid();

                    //close any open file descriptors so it's not touched and we can properly redirect
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    close(STDERR_FILENO);

                    //redirect stdin to /dev/null so we don't need the shell to get in
                    int devnull = open("/dev/null", O_WRONLY);
                    dup2(devnull, STDIN_FILENO);
                    close(devnull);

                    //execute the command in the background by execvping again but with new array (to avoid argument of itself i.e ls ls)
                    char *cmdArgs[MAX_ARG];
                    int i, j;
                    for(i = 0, j = 0; i < count; i++) {
                        if(strcmp(inArgs[i], "<") == 0) {
                            redirectIn(inArgs, i);
                            i++;
                        } else if(strcmp(inArgs[i], ">") == 0) {
                            redirectOut(inArgs, i);
                            i++;
                        } else {
                            cmdArgs[j] = inArgs[i];
                            j++;
                        }
                    }

                    //set j as end of list
                    cmdArgs[j] = NULL;

                    //execvp here
                    int error = execvp(cmdArgs[0], cmdArgs);
                    if(error == -1) {
                        perror("execvp");
                        exit(1);
                    }
            }      

                return;

        default:
            //waits till child is done then kills

            if(background == 1 && foreground_only_mode != 1){
                waitpid(pid, &status, WNOHANG);
            }
            else{
                waitpid(pid, &status, 0);
            }
            
            if(background == 1){
                if (WIFEXITED(status)){
                printf("exit value %d\n", WEXITSTATUS(status));
                printf("background PID is: %d\n", pid);
                }
                else if (WIFSIGNALED(status)){
                    printf("terminated by signal %d\n", WTERMSIG(status));
                    printf("background PID is: %d\n", pid);
                }
                else{
                    printf("unknown termination value or signal %d\n", WTERMSIG(status));
                    printf("background PID is: %d\n", pid);
                } 
            }
        }
}

void redirectOut(char * inArgs[MAX_ARG], int i){
    //this opens a file using the name passed in and gives us the ability to write and other to read
    //then we write unless there's an error then we error out
    int fd = open(inArgs[i+1], O_CREAT | O_RDWR | O_TRUNC, 0644);
                    if (fd == -1) {
                        //error handling
                        perror("open");
                        printf("\ncannot open %s for use\n",inArgs[i+1]);
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
}

void redirectIn(char * inArgs[MAX_ARG], int i){
    //this opens a file using the name passed in and gives us the ability read it
    //then we write unless there's an error then we error out
        int fd = open(inArgs[i+1], O_RDONLY);
                    if (fd == -1) {
                        //error handling
                        perror("open");
                        printf("cannot open %s for use\n",inArgs[i+1]);
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd); 
}