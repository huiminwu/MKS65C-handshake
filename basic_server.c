#include "pipe_networking.h"
#include <signal.h>
#include <ctype.h>

static void sighandler(int signo) {
    if (signo == SIGINT) {
        printf("\nremoving wkp\n");
        remove("wkp");
        printf("Program ending due to SIGINT\n");
        exit(0);
    }
}

int main() {

    //should end when interrupted with ctrl-c
    signal(SIGINT, sighandler);
    int to_client;
    int from_client;
    char * processed = malloc(sizeof(char *) * BUFFER_SIZE);
    int i;


    while(1) {
        from_client = server_handshake( &to_client );
        if (from_client != 0) { //if connected to a child server
            while (read(from_client, processed, BUFFER_SIZE)) {
                printf("getting data from client\n");
                printf("processing data\n");
                printf("before: %s ", processed);
                for (i = -0;i < (int)strlen(processed);i++){
                    processed[i] = toupper(processed[i]);
                }

                printf("after: %s\n", processed);

                if (write(to_client, processed, BUFFER_SIZE) == -1) {
                    perror("write");
                } else {
                    printf("sending processed data back to client\n");
                }
            }
            printf("Client disconnected\n");
        }
    }
    free(processed);
    return 0;
}
