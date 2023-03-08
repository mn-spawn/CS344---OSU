/* key gen takes in an arg and then give back a file that has the number specified 
random chars
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]);
    int rand_num;
    srand(time(NULL));

    int i = 0;

    for(i = 0; i < num; i++){
        rand_num = rand() % 27; 

        //if it's 26 then get space
        if(rand_num == 26) {
            printf(" ");

        //else get the actual letter
        } else {
            printf("%c", rand_num + 'A');
        }
    }

    //add the newline
    printf("\n");
    return 0;
}