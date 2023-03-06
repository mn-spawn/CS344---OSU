#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()


//-----------------------------------------------------------------------------//

void error(const char *msg) { 
  perror(msg); 
  exit(0); 
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
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[256];
    // Check usage & args
    if (argc < 3) { 
        fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
        exit(0); 
    } 

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 

    if (socketFD < 0){
        error("CLIENT: ERROR opening socket");
    }

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        error("CLIENT: ERROR connecting");
    }
    
    //clear out the buffer array
    memset(buffer, '\0', sizeof(buffer));
    
    //put in the args to buffer going to server
    strcpy(buffer, argv[1]);
    strcat(buffer, "\n");
    strcat(buffer, argv[2]);

    //send message to server
    //write to the server
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
    if (charsWritten < 0){
        error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(buffer)){
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

    //get return message from server
    //clear out the buffer again for reuse
    memset(buffer, '\0', sizeof(buffer));
    //read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("CLIENT: ERROR reading from socket");
    }

    //this puts the cipher text into cipher text file
    printf(buffer);

    //close the socket
    close(socketFD); 
    return 0;
}
