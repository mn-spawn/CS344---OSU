#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>


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
    if(!movies){printf("Failure to open file. Try checking your file path or name"); return 0;}
    else{printf("%s %s", "\nOpened your file named", fileName);};
    printf("\n");

    /* Here is where we start proccessing - inspo from student main provided */
    char *currLine = NULL;
    size_t len = 0;
    char *token;
    size_t nread;

    /* linked list structure */
    struct movie *head = NULL;
    struct movie *tail = NULL;

    int count = 0;
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
    return head;
}

char * makeDirectory(){
    srand(time(NULL));
    int random = rand() % 99999;

    char * randomString = malloc(sizeof(char) * 5);

    sprintf(randomString, "%d", random);
    
    char * pathName = malloc(sizeof(char) * 20);
    strcpy(pathName,"spawnm.movies.");

    strcat(pathName, randomString);

    mkdir(pathName, 0750);
    printf("%s %s", "Created a directory named:", pathName);

    return pathName;

}

void parseYears(struct movie *head, char * pathname){
    struct movie *temp = head->next;
    int movYear;
    char * moviePath = malloc(sizeof(char)*10);
    
   while(temp != NULL){
        sscanf(temp->year, "%d", &movYear);
        char * movieYear = malloc(sizeof(char) * 4);
        sprintf(movieYear, "%d", movYear);
        int arrayIndex = (movYear-1900); 

        sprintf(moviePath, "./%s.txt", movieYear);

        char * directoryName = malloc(strlen(pathname)+ sizeof(char)*3);
        sprintf(directoryName, "%s", "./");
        strcat(directoryName, pathname);
            
        chdir(directoryName);

    
        char * filePath = malloc(strlen(pathname) + sizeof(char)*10);

            FILE *fp;
            fp  = fopen (moviePath, "a");
            chmod(moviePath, 0640);

            strcat(temp->title, "\n");

            fputs(temp->title,fp);
            if(!fp){
                perror("Error");            
            }
            close(fp);
    

        temp = temp->next;
}
}

int getSizeComp(char *filename){
        /* this part opens the file */
    FILE *movies = fopen(filename, "r");
    if(!movies){
        perror("Error");
    }

    char *currLine = NULL;
    size_t len = 0;
    char *token;
    size_t nread;

    int count = 0;

    while((nread = getline(&currLine, &len, movies)) != -1){  
        count += 1;
    }
        fclose(movies);
    

    return count;
}

/* option helper functions */
void optionOne(){
   printf("You selected option one\n");

    DIR* currDir = opendir(".");
    struct dirent *aDir;
    
    int count = 0;
    char *name = malloc(sizeof(char)*1000);

    while((aDir = readdir(currDir)) != NULL){
        if(strncmp("movies_", aDir->d_name, strlen("movies_")) == 0){
            char * point = aDir->d_name + strlen(aDir->d_name);

            //from a stack overflow post
            if((point = strrchr(aDir->d_name,'.')) != NULL ) {
                if(strcmp(point,".csv") == 0) {
                    int size = getSizeComp(aDir->d_name);
                    if(size > count){
                        strncpy(name, aDir->d_name, 1000);
                        count = size;
                    }
                }
            }
        } 
    }

    printf("%s %s", "Now processing largest file named:",name);
    struct movie *head = readFile(name);
    char * pathname = makeDirectory();

    parseYears(head, pathname);
    free(pathname);
    free(currDir);
    free(aDir);
   
}

void optionTwo(){
    printf("You selected option two\n");
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    
    int count = INT_MAX;
    char *name = malloc(sizeof(char)*1000);

    while((aDir = readdir(currDir)) != NULL){
        if(strncmp("movies_", aDir->d_name, strlen("movies_")) == 0){
            char * point = aDir->d_name + strlen(aDir->d_name);

            //from a stack overflow post
            if((point = strrchr(aDir->d_name,'.')) != NULL ) {
                if(strcmp(point,".csv") == 0) {
                    int size = getSizeComp(aDir->d_name);
    
                    if(count > size){
                        strncpy(name, aDir->d_name, 1000);
                        count = size;
                    }
                }
            }
        } 
    }

    printf("%s %s", "Now processing smallest file named:",name);
    struct movie *head = readFile(name);
    char * pathname = makeDirectory();

    parseYears(head, pathname);
    free(pathname);
    free(currDir);
    free(aDir);
   
}

void optionThree(char * fileName){
    
            struct movie *head = readFile(fileName);
            char * pathname = makeDirectory();

            parseYears(head, pathname);
}


int main(){

    /* 1 through 2 select options */
    bool valid = false;
    int selected = 0;
    while(selected != 4){
        selected = 0;
        fflush(stdin);
        while(!valid || selected == 0){
            valid = false;
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
            bool valid2 = false;
            while(valid2 != true)
            {
                printf("\nSelect either 1,2 or 3 from the options below:\n");
                printf("Enter 1 to pick the largest file\n");
                printf("Enter 2 to pick the smallest file\n");
                printf("Enter 3 to specify the name of a file\n");
                scanf("%d", &selected);

                if(selected == 1){valid2 = true; optionOne();}
                if(selected == 2){valid2 = true; optionTwo();}
                if(selected == 3){
                    printf("You selected option three\n");
                    printf("Enter your movie list file name: ");

                    char * fileName;
                    fileName=malloc(sizeof(char) * 20);
                    scanf("%s", fileName);
                    printf(fileName);
                    
                    FILE *movies = fopen(fileName, "r");
                    if(!movies){
                        printf("\nFailure to open file. Try checking your file path or name");
                    }
                    else{
                        valid2 = true;
                        optionThree(fileName);
                    }
                }
                if(selected < 0 || selected > 3){printf("You selected an invalid choice\n");}
                else{valid = 1;}
            }
        }
        
        else{return 0;}
    }
}



