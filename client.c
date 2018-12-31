#include "pipe_networking.h"
#include <signal.h>

static void sighandler(int signo) {
    if(signo == SIGINT) {
        printf("Disconnecting from server!\n");
        exit(0);
    }
}

int main() {
  //should end when interrupted with ctrl-c
  signal(SIGINT, sighandler);

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );
  char * input = malloc(sizeof(char) * 100);
  while(1) {

        printf("input: ");
        if(fgets(input, BUFFER_SIZE, stdin) == NULL ) {
            perror("fgets");
        } else {
            input[strlen(input) - 1] = '\0'; //need to add null to input
        }
        if (write(to_server, input, BUFFER_SIZE) == -1) {
            perror("write");
            exit(1);
        } else {
            printf("sending %s to server\n", input);
        }

        if (read(from_server, input, BUFFER_SIZE) == -1) {
            perror("read");
            exit(1);
        } else {
            printf("============================\n");
            printf("response from server: %s\n", input);
            printf("============================\n");
        }
    }
    free(input);
    return 0;
}
