#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_MTEXT_SIZE 100

// Structure for MQ
struct message {
    long msgtype; // should hold client Id
    int op;
    char mtext[100];
};

int main() {

    // The important stuff: Initialization of variables and Message queue declaration
    key_t key;
    int msgqid;
    struct message msg;
    int choice; 

    key = ftok(".", 'b');
    msgqid = msgget(key, 0666 | IPC_CREAT);

    if (msgqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
	
    // Dear client, apna naam-pata batao 
    printf("Enter Client-ID:");
    scanf("%ld",&msg.msgtype);
    
    
    while(1)
    {
    	
    	printf("\nMenu:");
    	printf("Enter 1 to contact the Ping Server\n");
    	printf("Enter 2 to contact the File Search Server\n");
    	printf("Enter 3 to contact the File Word Count Server\n");
    	printf("Enter 4 if this Client wishes to exit\n");
    	printf("Enter choice:");
    	scanf("%d",&choice);
    	
    	
    	// Message augmentation
    	if(choice==1)
    	{
    		strcpy(msg.mtext,"Hi");
    	}
    	else if(choice==2||choice==3)
    	{
    		printf("Enter file name:");
    		scanf("%s",msg.mtext);
    		
    		
    	}
    	else if(choice==4)
    	{
    		strcpy(msg.mtext,"4");
    	}
    	else
    	{
    		continue;
    	}
    	
    	msg.op=choice;
    	// Send message to main server
    	if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1)
    		{
    			//Error in sending Message
    			perror("msgsnd");
    			exit(EXIT_FAILURE);
    		}
    	
    	printf("\nMessage sent to server");
    	
    	// Waiting reply from server
    	if(msgrcv(msgqid,&msg,MAX_MTEXT_SIZE,msg.msgtype,0)==-1)
    	{
    		perror("msgrcv");
    		exit(EXIT_FAILURE);
    	}
    	
    	printf("\nServer Reply: %s\n",msg.mtext);
    	
    	if(choice==4)
    		break;
    	
    }
    

    return 0;
}
