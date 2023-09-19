#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char input[100];

    // Create a pipe to receive input from Process 1
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Read input from the pipe
    read(pipefd[0], input, sizeof(input));

    close(pipefd[0]); // Close the read end of the pipe

    printf("Process 2: Received input from Process 1: %s\n", input);

    return 0;
}
