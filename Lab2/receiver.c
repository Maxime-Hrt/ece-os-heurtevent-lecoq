#include "stdio.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "string.h"

#define MESSAGE_SIZE 256

struct message {
    long mtype;
    char mtext[MESSAGE_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;

    //Get the key from the sender.c file
    key = ftok("sender.c", 'A');

    //Get the message queue id
    msgid = msgget(key, 0666 | IPC_CREAT);

    //Receive the message
    msgrcv(msgid, &msg, sizeof(msg), 1, 0);

    //Print the message
    printf("Message received from sender: %s\n", msg.mtext);

    //Remove the message queue
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}