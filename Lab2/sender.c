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

    //Generate a unique key
    key = ftok("sender.c", 'A');

    //Get the message queue id
    msgid = msgget(key, 0666 | IPC_CREAT);


    msg.mtype = 1;
    printf("Enter a message to send to receiver: ");
    fgets(msg.mtext, MESSAGE_SIZE, stdin);

    //Send the message
    msgsnd(msgid, &msg, sizeof(msg), 0);

    printf("Message sent to receiver: %s\n", msg.mtext);

    return 0;
}