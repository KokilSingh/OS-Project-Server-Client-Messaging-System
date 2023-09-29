#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

#define MAX_MTEXT_SIZE 100

// Structure for MQ
struct message {
    long msgtype; // should hold client Id
    char mtext[100];
};

int main() {
    
    // The important stuff: Initialization of variables and Message queue declaration
    key_t key;
    int msgqid;
    struct message msg;
   
    long clientID;
    
    key = ftok(".", 'b');
    msgqid = msgget(key, 0666 | IPC_CREAT);

    if (msgqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
	char choice;
    // Listen for server closure requests
    while(1)
    {
    
    	printf("Do you want to shut down the main server. Enter Y for Yes or N for No:\n");
	scanf("%c",&choice);
	
	if(choice=='Y' || choice=='y')
	{
		break;
	}	
    	else
    	{
    		continue;
    	}
    	
    }
	
	// Now Cleanup initiation
	// Send message to main server
	msg.msgtype=999;
	strcpy(msg.mtext,"-1");
    	if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1)
    		{
    			//Error in sending Message
    			perror("msgsnd");
    			exit(EXIT_FAILURE);
    		}
    	
    	printf("\nMessage sent to server... Gracefully Terminating\n");
    

    return 0;
}

