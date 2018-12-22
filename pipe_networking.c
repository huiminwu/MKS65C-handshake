#include "pipe_networking.h"
#include <ctype.h>
#include <signal.h>

/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
    remove("wkp");
    if(mkfifo("wkp", 0666) == -1) {
        perror("mkfifo");
        exit(1);
    } else {
        printf("wkp server created\n");
    }
    
    int up = open("wkp", O_RDONLY);
    if (up == -1) {
        perror("open");
    } else {
        printf("connected!\n");
    }

    char * pfifo = malloc(sizeof(char *) * BUFFER_SIZE);
    printf("recieving client's msg\n");
    if (read(up, pfifo, BUFFER_SIZE) == -1) {
        perror("read");
        exit(1);
    }

    //printf("removing wkp\n");
    //remove("wkp");
    printf("connecting to client fifo\n");
    *to_client = open(pfifo, O_WRONLY);
    if (*to_client != -1) {
        printf("successful connection!\n");
    } else {
        printf("no connection!\n");
    }

    printf("sending ack\n");
    if(write(*to_client, ACK, BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent initial msg!\n");
    }


    //===============================================================
    // End of Handshake
    //===============================================================
    
    //next few blocks to display client's server is just a double check
    char * cli_resp = malloc(sizeof(char *) * BUFFER_SIZE);
    read(up, cli_resp, BUFFER_SIZE);
    
    printf("======================\n");
    printf("client's reply: %s\n", cli_resp);
    printf("======================\n");
    //printf("donezo\n");


    //if(close(*to_client) == -1) {
    //    perror("close");
    //} else {
    //    printf("closed connections to the client\n");
    //}

    //free(cli_resp);
    //close(up);
    return up;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
    if(mkfifo("priv", 0666) == -1) {
        perror("mkfifo");
        exit(1);
    } else {
        printf("private fifo made!\n");
    }

    *to_server = open("wkp", O_WRONLY);
    if (*to_server == -1) {
        perror("open");
    } else {
        printf("client has connected to server\n");
    }
    
    if(write(*to_server, "priv", BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent private fifo name\n");
    }

    int down = open("priv", O_RDONLY);
    char * buffer = malloc(sizeof(char *) * BUFFER_SIZE);
    if (buffer == NULL) {
        printf("error w malloc\n");
    }

    if (read(down, buffer, BUFFER_SIZE) == -1){
        perror("read");
        exit(1);
    } else {
        printf("Received server's msg!\n");
    }

    printf("removing private fifo\n");
    remove("priv");

    printf("======================\n");
    printf("server's msg: %s\n", buffer);
    printf("======================\n");

    if (write(*to_server, ACK, BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent response to server\n");
    }
   
    //=====================================================
    // End of Handshake
    //=====================================================
    return down;
}
