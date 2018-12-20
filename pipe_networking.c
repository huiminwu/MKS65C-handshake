#include "pipe_networking.h"
#include <ctype.h>
#include <signal.h>

static void sighandler(int signo) {
    if (signo == SIGINT) {
        printf("\nremoving wkp\n");
        remove("wkp");
        printf("Program ending due to SIGINT\n");
        exit(0);
    }
}
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

    while(1) {
        if (read(up, cli_resp, BUFFER_SIZE) == -1) {
            perror("read");
        } else {
            printf("getting data from client\n");
        }

        printf("processing data\n");
        printf("before: %s ", cli_resp);
        char * processed = malloc(sizeof(char *) * BUFFER_SIZE);
        int i;
        for (i = -0;i < (int)strlen(cli_resp);i++){
            processed[i] = toupper(cli_resp[i]);
        }

        printf("after: %s\n", processed);

        if (write(*to_client, processed, BUFFER_SIZE) == -1) {
            perror("write");
        } else {
            printf("sending processed data back to client\n");
        }
    }

    if(close(*to_client) == -1) {
        perror("close");
    } else {
        printf("closed connections to the client\n");
    }

    free(cli_resp);
    close(up);
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

    if (write(*to_server, "hi serve", BUFFER_SIZE) == -1) {
        perror("write");
        exit(1);
    } else {
        printf("sent response to server\n");
    }
   
    //=====================================================
    // End of Handshake
    //=====================================================
    printf("If you ctrl-c the server will also close because I can't figure out how to reset it :'(\n"); 
    while(1) {
        //should end when interrupted with ctrl-c
        signal(SIGINT, sighandler);

        printf("input: ");
        char * input = malloc(sizeof(char) * 100);
        if(fgets(input, 100, stdin) == NULL ) {
            perror("fgets");
        }

        if (write(*to_server, input, BUFFER_SIZE) == -1) {
            perror("write");
            exit(1);
        } else {
            printf("sending %s to server\n", input);
        }

        if (read(down, buffer, BUFFER_SIZE) == -1) {
            perror("read");
            exit(1);
        } else {
            printf("============================\n");
            printf("response from server: %s\n", buffer);
            printf("============================\n");
        }
        free(input);
    }
   
    //free(buffer);
    //close(*to_server);
    close(down);
    return down;
}
