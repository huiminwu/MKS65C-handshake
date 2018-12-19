#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
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

    char * buff = malloc(sizeof(char *) * BUFFER_SIZE);
    printf("recieving client's msg\n");
    if (read(up, buff, BUFFER_SIZE) == -1) {
        perror("read");
        exit(1);
    }

    printf("removing wkp\n");

    printf("connecting to client fifo\n");
    *to_client = open(buff, O_WRONLY);
    if (*to_client != -1) {
        printf("successful connection!\n");
    } else {
        printf("no connection!\n");
    }

    printf("sending ack\n");
    if(write(*to_client, ACK, HANDSHAKE_BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent initial msg!\n");
    }
    
    char * cli_resp = malloc(sizeof(char *) * HANDSHAKE_BUFFER_SIZE);
    read(up, cli_resp, HANDSHAKE_BUFFER_SIZE);

    printf("======================\n");
    printf("client's reply: %s\n", cli_resp);
    printf("======================\n");
    printf("donezo\n");

    if(close(*to_client) == -1) {
        perror("close");
    } else {
        printf("closed connections to the client\n");
    }

    free(buff);
    free(cli_resp);
    close(up);
    remove("wkp");
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
    char * buffer = malloc(sizeof(char *) * HANDSHAKE_BUFFER_SIZE);
    if (*buffer == NULL) {
        printf("error w malloc\n");
    }

    if (read(down, buffer, HANDSHAKE_BUFFER_SIZE) == -1){
        perror("read");
        exit(1);
    } else {
        printf("Received server's msg!\n");
    }

    printf("======================\n");
    printf("server's msg: %s\n", buffer);
    printf("======================\n");

    if (write(*to_server, "hi serve", HANDSHAKE_BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent response to server\n");
    }
    
    free(buffer);
    close(*to_server);
    close(down);
    remove("priv");
    return down;
}
