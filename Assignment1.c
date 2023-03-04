#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/*movie struct*/
struct movie{
    char* title;
    char* year;
    char* languages;
    char* rating;

    struct movie *next;

};

/* strong inspiration for from the student main.c */
struct movie *createMovie(char *currLine){
    struct movie *currMovie = malloc(sizeof(struct movie));

    char *savePtr;

    /*parse title */
    char *token = strtok_r(currLine, ",", &savePtr);
    currMovie->title = calloc(strlen(token)+1,sizeof(char));
    strcpy(currMovie->title, token);

    /* parse year */
    token = strtok_r(NULL, "," , &savePtr);
    currMovie->year = calloc(strlen(token)+1,sizeof(char));
    strcpy(currMovie->year, token);

    /* parse languages*/
    token = strtok_r(NULL, ",", &savePtr);
    currMovie->languages = calloc(strlen(token)+1,sizeof(char));
    strcpy(currMovie->languages, token);

    /* parse rating */
    token = strtok_r(NULL, ",", &savePtr);
    currMovie->rating = calloc(strlen(token)+1,sizeof(char));
    strcpy(currMovie->rating, token);

    currMovie->next = NULL;
}

/* helper functions */
void printMovie(struct movie *mov){
  printf("%s, %s %s, %s\n", 
            mov->title,
            mov->year,
            mov->languages,
            mov->rating);
}

void printList(struct  movie *head){
    struct movie *temp = head->next;
    while(temp != NULL){
        printMovie(temp);
        temp = temp->next;
    }
}

struct movie * readFile(char *fileName){
    
    /* this part opens the file */
    FILE *movies = fopen(fileName, "r");
    //if(!movies){printf("Failure to open file. Try checking your file path or name\n"); return;}
    //else{printf("%s %s", "Opened your file named", fileName);}
    //printf("\n");

    /* Here is where we start proccessing - inspo from student main provided */
    char *currLine = NULL;
    size_t len = 0;
    char *token;
    size_t nread;

    /* linked list structure */
    struct movie *head = NULL;
    struct movie *tail = NULL;

    int count;
    struct linkedList *movieList;

    /* reading the file here */
    while((nread = getline(&currLine, &len, movies)) != -1){

       
        struct movie *newMovie = createMovie(currLine);
        if(head == NULL){head = newMovie; tail= newMovie;}
        else{
            tail->next = newMovie;
            tail = newMovie;
        }
        count += 1;
    }
        fclose(movies);

    printf("%s %d %s", "\nProcessed data for", count-1, "movies\n");
    //also print name
}

/* option helper functions */
void optionOne(){
   printf("You selected option one\n");
   printf("Now processing largest file named:");
}

void optionTwo(){
    printf("You selected option two\n");
   printf("Now processing smallest file named:");
}

void optionThree(){
    printf("You selected option three\n");
    printf("Enter your movie list file name: ");

    char * fileName;
    fileName=malloc(sizeof(char) * 20);
    scanf("%s", fileName);
    printf(fileName);
   struct movie *newMovie = readFile(fileName);
}





int main(){

    /* 1 through 2 select options */
    bool valid = false;
    int selected = 0;
    while(selected != 4){
        selected = 0;
        fflush(stdin);
        while(!valid || selected == 0){
            printf("\nSelect either 1 or 2 from the options below:\n");
            printf("1.Select file to process\n");
            printf("2.Quit\n");
            scanf("%d", &selected);
            
            //checking if integer is in desired range
            if(selected < 0 || selected > 2){printf("You selected an invalid choice");}
            else{valid = 1;}
        }

        /* option 1-4 actions */
        if(selected == 1){
            int year;
            printf("You selected 1\n");

            printf("\nSelect either 1,2 or 3 from the options below:\n");
            printf("Enter 1 to pick the largest file\n");
            printf("Enter 2 to pick the smallest file\n");
            printf("Enter 3 to specify the name of a file\n");
            scanf("%d", &selected);

            if(selected == 1){optionOne();}
            if(selected == 2){optionTwo();}
            if(selected == 3){optionThree();}
            if(selected < 0 || selected > 3){printf("You selected an invalid choice\n");}
            else{valid = 1;}
        }
        
        else{return 0;}
    }
}



