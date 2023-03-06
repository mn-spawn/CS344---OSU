/* 
Write a program that creates 4 threads to process input from standard input as follows

Thread 1, called the Input Thread, reads in lines of characters from the standard input.
Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
Thread 3, called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
Thread 4, called the Output Thread, write this processed data to standard output as lines of exactly 80 characters.
Furthermore, in your program these 4 threads must communicate with each other using the Producer-Consumer approach. 
*/

/* special symbols
/n - newline
STOP - stop running
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <math.h> // must link with -lm

#define BUFF_SIZE 1000
#define NUM_LINES 50

//need functions for each thread thing and buffers between some of them
//-----------------------------Buffers---------------------------------------------//

//initing the buffers and mutexes etc
char * bufferinLs[BUFF_SIZE];
int countinLs = 0;
int putinLs = 0;
int getinLs = 0;
pthread_mutex_t inLs = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inLsCond = PTHREAD_COND_INITIALIZER;

char * bufferlsPs[BUFF_SIZE];
int countlsPs = 0;
int putlsPs = 0;
int getlsPs = 0;
pthread_mutex_t lsPs = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t lsPsCond = PTHREAD_COND_INITIALIZER;

char * bufferpsOp[BUFF_SIZE];
int countpsOp = 0;
int putpsOp = 0;
int getpsOp = 0;
pthread_mutex_t psOp = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t psOpCond = PTHREAD_COND_INITIALIZER;


//-----------------------------Buff 1---------------------------------------------//
//input to line separator buffer - (in to Ls)
void put_inLs_buff(char enter){
    //lock
    pthread_mutex_lock(&inLs);

    //put char in buffer
    ((char*)bufferinLs)[putinLs] = enter;

    //increment approriately
    putinLs++;
    countinLs++;

    //alert that data is in the buff
    pthread_cond_signal(&inLsCond);
    //unlock
    pthread_mutex_unlock(&inLs);
}

char * get_inLs_buff(){

    //wait while the buff is empty
    // while (countinLs == 0){
    //     pthread_cond_wait(&inLsCond, &inLs);
    // }
        
    // //get char from buf
    // char character = ((char*)bufferinLs)[putinLs];
 
    // //decrement appropriately
    // getinLs = getinLs + 1;
    // countinLs--;


    //lock
    pthread_mutex_lock(&inLs);

    // char* ret = (char*)&character;
    // printf(ret);
    // return ret;
}

//-----------------------------Buff 2---------------------------------------------//
//line separator to plus_sign buffer - (ls to Ps)
void put_lsPs_buff(char enter){
    //lock
    pthread_mutex_lock(&lsPs);

    //put char in buffer
    ((char*)bufferlsPs)[putlsPs] = enter;

    //increment approriately
    putlsPs++;
    countlsPs++;

    //alert that data is in the buff
    pthread_cond_signal(&lsPsCond);
    //unlock
    pthread_mutex_unlock(&lsPs);
}

char * get_lsPs_buff(){
    printf("get ls ps op");

    //wait while the buff is empty
    while (countlsPs == 0){
        pthread_cond_wait(&lsPsCond, &lsPs);
    }
        
    //get char from buff
    char character = ((char*)bufferlsPs)[putlsPs];
 
    //decrement appropriately
    getlsPs = getlsPs + 1;
    countlsPs--;

    //lock
    pthread_mutex_lock(&lsPs);
}

//-----------------------------Buff 3---------------------------------------------//
//plus sign to output buffer - (ps to Op)
void put_psOp_buff(char enter){
    printf("buff ps op");

    //lock
    pthread_mutex_lock(&psOp);

    ((char*)bufferlsPs)[putpsOp] = enter;
    putpsOp++;
    countpsOp++;

    //data is in the buff
    pthread_cond_signal(&psOpCond);
    //unlock
    pthread_mutex_unlock(&psOp);
}

char * get_psOp_buff(){
    printf("get buff ps op");

    //wait while the buff is empty
    while (countpsOp == 0){
        pthread_cond_wait(&psOpCond, &psOp);
    }
        
    //get char from buff
    char character = ((char*)bufferpsOp)[putpsOp];
 
    //decrement appropriately
    getpsOp = getpsOp + 1;
    countpsOp--;

    //lock
    pthread_mutex_lock(&psOp);

    printf("%c", character);
    char * ret = "h";
    return ret;
}

//----------------------------------------------------------------------------------//
//-----------------------------Thread 1---------------------------------------------//

char * get_user_input(){
  char * value = malloc(sizeof(char) * BUFF_SIZE);
  printf("\nEnter your characters:\n");

    while (fgets(value, BUFF_SIZE, stdin)) {
        // Check for "STOP" on a line by itself
        if (strcmp(value, "STOP\n") == 0) {
            break;
        }
        
        // Process input line
        printf("%s", value);
    }

  return value;
}

void * get_input(){
    char * chars = get_user_input();
    printf(chars);
    // for(int i = 0; i < BUFF_SIZE; i++){
    //     //put_inLs_buff(chars[i]);
    //     printf("%c", chars[i]);
    // }

    return NULL;
}

//-----------------------------Thread 2---------------------------------------------//

//logic get input from inLs then use if logic to replace '\n' with ' '
void * line_separator(){
    //printf("get ls");

    // char * buff = malloc(sizeof(char)* BUFF_SIZE);
    // while(countinLs != 0)
    // {
    //   printf("%c",get_inLs_buff());
    //   //put_buff_2(square_root);
    // }
    // return NULL;
}

//-----------------------------Thread 3---------------------------------------------//

void * plus_sign(){
    //printf("get ps");  
;
}

//-----------------------------Thread 4---------------------------------------------//

void * write_output(){
    char * line = malloc(sizeof(char)* BUFF_SIZE);
    line = get_psOp_buff();
    //printf(line);
    return NULL;
}


//-----------------------------Main Is Below---------------------------------------------//

//took this from the provided code - 6_5_prod_cons_pipeline.c
int main(){
    srand(time(0));

    //here init the 4 threads we need
    pthread_t input_t;
    pthread_t line_separator_t;
    pthread_t plus_sign_t;
    pthread_t output_t;

    //create them
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_separator_t, NULL, line_separator, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);

    //wait till they terminate then joing
    pthread_join(input_t, NULL);
    pthread_join(line_separator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);

    //exit
    return EXIT_SUCCESS;
}