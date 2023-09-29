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
    long mtype;
    int oprid;
    char mfile[100];
};

int main()
{
    key_t key;
    int msgqid;
    int clientid = 999;
    char input_file[100];
    

    key = ftok(".",'a');
    msgqid = msgget(key, 0664 | IPC_CREAT);


    while(true)
    {
    	
        char choice;
        printf("Do you want the server to terminate?\nPress Y for Yes and N for No\n");
        scanf(" %c",&choice);
        if(choice == 'N' || choice == 'n')
        {
            sleep(5);
        }
        else if(choice == 'Y' || choice == 'y')
        {
            struct message msg;
            strcpy(msg.mfile, "");
            msg.mtype = clientid;
            msg.oprid = -1;
            msgsnd(msgqid,&msg,sizeof(msg.mfile),0);
            printf("Server will be terminated gracefully\nThank You\n");
            break;
        }
        else
        {
            printf("Incorrect Input, Please Try Again\n");
        }
    }

    

    return 0;
}
