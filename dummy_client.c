#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stdbool.h>

struct message{
    int mtype;
    int oprid;
    char mfile[100];
};

int main(){
    key_t key;
    int msgqid;
    int clientid;
    int operatorid;
    char input_file[100];

    key = ftok(".",'a');
    msgqid = msgget(key, 0664 | IPC_CREAT);

    printf("Enter Client ID: ");
    scanf("%d",&clientid);


    while(true){
        printf("1. Enter 1 to contact the Ping Server\n");
        printf("2. Enter 2 to contact the File Search Server\n");
        printf("3. Enter 3 to contact the File Word Count Server\n");
        printf("4. Enter 4 if this Client wishes to exit\n");
        printf("Enter Number: ");

        scanf("%d",&operatorid);
        if(operatorid == 4){
            break;
        }

        else if(operatorid == 1){
            char input_str[100];
            scanf("%s",input_str);
            struct message msg;
            strcpy(msg.mfile, "");
            msg.mtype = clientid;
            msg.oprid = 1;
            msgsnd(msgqid,&msg,sizeof(msg),0);

            struct msqid_ds info;
            msgctl(msgqid, IPC_STAT, &info);
            while(info.msg_qnum == 1){
                msgctl(msgqid, IPC_STAT, &info);
            }
            struct message msg1;
            msgrcv(msgqid,&msg1,sizeof(msg1),0,0);
            printf("\n%s\n",msg1.mfile);
            struct message remove;
            while (msgrcv(msgqid, &remove, sizeof(remove), 0, IPC_NOWAIT) != -1) {
                // Remove each message from the queue until it's empty
            }
        }

        else if(operatorid == 2 || operatorid == 3){
            printf("Enter File Name: ");
            scanf("%s",input_file);
            struct message msg;
            msg.mtype = clientid;
            strcpy(msg.mfile,input_file);
            msg.oprid = operatorid;
            msgsnd(msgqid,&msg,sizeof(msg),0);
            //printf("Message Sent Successfully!!!\n");

            struct msqid_ds info;
            msgctl(msgqid, IPC_STAT, &info);
            while(info.msg_qnum == 1){
                msgctl(msgqid, IPC_STAT, &info);
            }
            struct message msg1;
            msgrcv(msgqid,&msg1,sizeof(msg1),0,0);
            if(operatorid == 2 && strlen(msg1.mfile) == 0){
                printf("\nThis file doesn't exist\n");
            }
            else{
                printf("\n%s\n",msg1.mfile);
            }
            struct message remove;
            while (msgrcv(msgqid, &remove, sizeof(remove), 0, IPC_NOWAIT) != -1) {
                // Remove each message from the queue until it's empty
            }
        }
    }
}
