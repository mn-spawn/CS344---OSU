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

void printList(struct movie *head){
    struct movie *temp = head->next;
    while(temp != NULL){
        printMovie(temp);
        temp = temp->next;
    }
}

/* option helper functions */
void optionOne(int year, struct movie *head){
    struct movie *temp = head;
    int movYear;
    int count = 0;
    /* this gets the year for each movie and only prints if matches desired year*/
    while(temp != NULL){
        sscanf(temp->year, "%d", &movYear);
        if(movYear == year){
            printMovie(temp);
            count +=1;
        }
        temp = temp->next;
    }
    if(count == 0){printf("There are no movies from this year\n");}
}

void optionTwo(struct movie *head){
    /* this function creates an array that corresponds to every yesr (2021-1900)
        then calcualtes the year and puts the movie in the index if empty
        otherwise it compares the rating of the current movie and the movie
        in the index
    */
    struct movie *temp = head->next;
    struct movie *yearArray[121];

   int movYear;
   double movRating;
   double arrayRating;
    int i = 0;
    for(i; i<=121;i++){
            yearArray[i] = NULL;
        }

   while(temp != NULL){
        sscanf(temp->year, "%d", &movYear);
        int arrayIndex = (movYear-1900);    

        //put index in if empty
        if(yearArray[arrayIndex] == NULL){
            yearArray[arrayIndex] = temp;
        }
        //otherwise compare rating of array movie and current movie
        else{
            sscanf(temp->rating, "%f", &movRating);
            char * rating = temp->rating;
            char * array =  yearArray[arrayIndex]->rating;
            sscanf(rating, "%lf", &movRating);
            sscanf(array, "%lf", &arrayRating);

            if(movRating > arrayRating){
                yearArray[arrayIndex] = temp;
            }
        }
        temp = temp->next;
   }
    int j = 0;

    //print only if the array index is not empty (a movie for that year exists)
    for(j; j <121; j++){

        if(yearArray[j] != NULL){
            double holder;
            sscanf(yearArray[j]->rating, "%lf", &holder);
            printf("%s %0.1f %s\n",
                yearArray[j]->year,
                holder, 
                yearArray[j]->title);
        }

    }
}

void optionThree(char * language, struct movie *head){
    /* this checks if there is a substrin of the language in main list if yes it check that the substring
        ends in either ; or ] so we know it is a language (so Engl doesn't show up as a language for example)
    */
    struct movie *temp = head->next;
    int count = 0;
    while(temp != NULL){
        char * l = temp->languages;
        char * getLan = strstr(l, language);
        int size = strlen(language);
    
        if(getLan != NULL){
            if(*(getLan+size) == ';' || *(getLan+size) == ']'){
                printMovie(temp);
                count +=1;
            }
            
        }
        temp = temp->next;
    }
    if(count == 0){
            printf("No movies in that language on this list\n");
    }
}




int main(int argc, char *argv[]){
    /* this part takes an input file */
    if (argc == 1){printf("You didn't provide a file path", "\n");}
    if(argc > 2){printf("Too many arguments provided");}
    
    /* this part opens the file */
    FILE *movies = fopen(argv[1], "r");
    if(!movies){printf("Failure to open file. Try checking your file path or name");}
    else{printf("%s %s", "Opened your file named", argv[1]);}
    printf("\n");

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

    /* 1 through 4 select options */
    bool valid = false;
    int selected = 0;
    while(selected != 4){
        selected = 0;
        fflush(stdin);
        while(!valid || selected == 0){
            printf("\nSelect a number 1-4 from the options below:\n");
            printf("1.Show movies released in the specified year\n");
            printf("2.Show highest rated movie for each year\n");
            printf("3.Show the title and year of release of all movies in a specific language\n");
            printf("4.Exit from the program\n");
            printf("\nEnter:");
            scanf("%d", &selected);
            
            //checking if integer is in desired range
            if(selected < 0 || selected > 5){printf("You selected an invalid choice");}
            else{valid = 1;}
        }

        /* option 1-4 actions */
        if(selected == 1){
            int year;
            printf("You selected 1\n");
            printf("\nEnter a year: ");
            scanf("%d", &year);

            optionOne(year, head);

        }
        if(selected == 2){
            printf("You selected 2\n");
            optionTwo(head);

        }
        if(selected == 3){
            
            char *language; 
            language=malloc(sizeof(char) * 20);
            printf("You selected 3\n");
            printf("\nEnter a language:");
            scanf("%s", language);
            
            optionThree(language, head);
        }
    }
    /* if 4 is pressed it returns here */
    return 0;
}



