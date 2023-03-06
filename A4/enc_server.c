#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

//-----------------------------------------------------------------------------//

int checker(char *plaintext, char *key){
  //this function checks to make sure all valid chars and length is sufficient
    int i;
    int len = strlen(plaintext);
    int lenKey = strlen(key);

    if(len > lenKey){
        //if key is not long enough
        printf("\nERROR: key length insufficient\n");
        return 1;
    }

    for (i = 0; i < len-1; i++) {
        char c = plaintext[i];
        char k = key[i];

        if ((k < 'A' && k > 'Z' || k != ' ') && (c < 'A' && c > 'Z' || c != ' ')) {
            printf("%s on chars %c and %c\n", "\nERROR: invalid character in either message or key", c,k);
            return 1;
        }
    }

    return 0;
}

char * encode(char *plaintext, char *key) {
    int i;
    int len = strlen(plaintext);

    char *ciphertext = malloc(sizeof(char) * (len + 1));

    for (i = 0; i < len; i++) {
        char c = plaintext[i];
        char k = key[i];
        int cipher_val = (c + k) % 26;
        ciphertext[i] = 'A' + cipher_val;
    }

    ciphertext[len+1] = '\n';

    return ciphertext;
}

char *read_file(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error: Could not open file.\n");
        exit(1);
    }

    // Get the size of the file
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    // Allocate memory for the buffer
    char *buffer = malloc(size + 1);

    // Read the contents of the file into the buffer
    fread(buffer, 1, size, file);

    // Add a null terminator to the end of the buffer
    buffer[size] = '\0';

    // Close the file
    fclose(file);

    // Return the buffer
    return buffer;
}
//-----------------------------------------------------------------------------//

//error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

//----------------------------------------------------------------------//

//set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
 
  //clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  //the address should be network capable
  address->sin_family = AF_INET;
  //store the port number
  address->sin_port = htons(portNumber);
  //allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

//----------------------------------------------------------------------//

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    // Get the message from the client and display it
    memset(buffer, '\0', 256);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, 255, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }

    char *newline_pos = strchr(buffer, '\n');
    char *arg1;
    char *arg2;
    // If a newline was found, extract the two arguments
    if (newline_pos != NULL) {
        // Null-terminate the string at the newline character
        *newline_pos = '\0';

        // Extract the first argument
        arg1 = buffer;
        // Advance the pointer to the character after the newline
        arg2 = newline_pos + 1;
    }

    printf("SERVER: I received these args from the client");
    char * message = read_file(arg1);
    char * key = read_file(arg2);

    char * ciphertext = malloc(sizeof(char)* strlen(message));

    checker(message, key);
    
    ciphertext = encode(message, key);
    // Send a Success message back to the client
    charsRead = send(connectionSocket, 
                    ciphertext, strlen(message), 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}