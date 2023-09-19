#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char input[100];
    printf("Process 1: Enter some text: ");
    fgets(input, sizeof(input), stdin);

    // Create a pipe to send the input to Process 2
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Write the input to the pipe
    write(pipefd[1], input, strlen(input) + 1);

    close(pipefd[1]); // Close the write end of the pipe

    return 0;
}
