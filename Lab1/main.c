#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "errno.h"

int mySystem(const char *command);

int main() {
    int choice = 0;
    char command[100];
    char buffer;

    while (choice != 5) {
        //Display menu
        printf("\n---Menu---\n");
        printf("1. Run program\n");
        printf("2. Kill a process\n");
        printf("3. List the file in the current folder\n");
        printf("4. Open application (On MacOS)\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Consommer le caractère de nouvelle ligne restant
        while ((buffer = getchar()) != '\n' && buffer != EOF);

        switch (choice) {
            case 1:
                printf("Enter the command name to run: ");
                scanf("%[^\n]", command);
                mySystem(command);
                break;
            case 2:
                printf("Enter the process ID to kill: ");
                int pid;
                scanf("%d", &pid);
                snprintf(command, sizeof(command), "kill %d", pid);
                mySystem(command);
                break;
            case 3:
                mySystem("ls");
                break;
            case 4:
                printf("Enter the application name to open: ");
                char app[100];
                scanf("%[^\n]", app);
                snprintf(command, sizeof(command), "/Applications/Figma.app/Contents/MacOS/%s", app);
                mySystem(command);
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice, try again\n");
        }
    }
    return 0;
}

int mySystem(const char *command){
    if (command == NULL || command[0] == '\0'){
        return -1;
    }

    int status;
    pid_t child_pid;

    child_pid = fork();

    //Error management
    if (child_pid < 0){
        perror("Fork failed");
        printf("Error Code: %d\n", errno);
    }

    if (child_pid == 0) {
        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("Exec failed");
        _exit(2);
    } else {
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -4;
        }
    }
}