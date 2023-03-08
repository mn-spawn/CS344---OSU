#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()


//-----------------------------------------------------------------------------//

int error(const char *msg) {
  perror(msg);
  return -1;
} 

//-----------------------------------------------------------------------------//

//set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
    //clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    //the address should be network capable
    address->sin_family = AF_INET;
    //store the port number
    address->sin_port = htons(portNumber);

    //get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname("localhost"); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
        exit(0); 
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, 
            hostInfo->h_addr_list[0],
            hostInfo->h_length);
  
}

//-----------------------------------------------------------------------------//

int main(int argc, char *argv[]) {

    //init
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[100000];

    // Check usage & args
    if (argc < 3) { 
        fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
        exit(1); 
    } 

    // Create a socket
    // Set up the server address struct
    // Connect to server
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0){error("CLIENT: ERROR opening socket");}

    setupAddressStruct(&serverAddress, atoi(argv[3]));
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        error("CLIENT: ERROR connecting");
    }
    
    //clear out the buffer array
    memset(buffer, '\0', sizeof(buffer));
    
    //put in the args to buffer going to server
    strcpy(buffer, argv[1]);
    strcat(buffer, "\n");
    strcat(buffer, argv[2]);
    strcat(buffer, "\n");
    strcat(buffer, argv[0]);


    //send message to server
    //write to the server
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
    if (charsWritten < 0){error("CLIENT: ERROR writing to socket");}
    if (charsWritten < strlen(buffer)){printf("CLIENT: WARNING: Not all data written to socket!\n");}

    //get return message from server
    memset(buffer, '\0', sizeof(buffer));
    int totalBytesRead = 0;
    while (totalBytesRead < sizeof(buffer) - 1) {
        int bytesToRead = sizeof(buffer) - 1 - totalBytesRead;
        if (bytesToRead > 5000) {
            bytesToRead = 5000;
        }
        charsRead = recv(socketFD, buffer + totalBytesRead, bytesToRead, 0);
        if (charsRead < 0) {
            error("CLIENT: ERROR reading from socket");
            break;
        } else if (charsRead == 0) {
            // connection closed
            break;
        }
        totalBytesRead += charsRead;
    }


    //this puts the cipher text into cipher text file
    if(strcmp(buffer, "bad")==0){
        char * bad = '\0';
        printf(bad);
    }
    else{
        strcat(buffer,"\n");
        printf(buffer);
    }

    //close the socket
    close(socketFD); 
    return 0;
}
