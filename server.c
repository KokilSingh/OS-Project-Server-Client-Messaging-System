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
	
    /********************************************************* GENERAL CASE LISTENING TO MESSAGES***********************************************/
    // Listen for client requests
    while(1)
    {
    
    	printf("Listening for messages\n");
    	
    	// Read whichever message is 1st in queue (kataar)
    	if(msgrcv(msgqid,&msg,MAX_MTEXT_SIZE,0,0)==-1)
    	{
    		perror("msgrcv");
    		exit(EXIT_FAILURE);
    	}
    	
    	clientID=msg.msgtype;
    	printf("Message received from client: %ld\n",msg.msgtype);
    	
    	
    	// Now that message is received, lets (drumrolls....) FORK IT
    	pid_t p=fork();
    	if(p==-1)
    	{
    		perror("fork");
    		exit(EXIT_FAILURE);
    	}
    	else if(p==0)
    	{
    		char response[100];
    		
    		// Bachha Process
    		
    		// Choice Handling
    		// Req: Need to create separate pipes for parent child communication
    		if(strcmp("4",msg.mtext)==0)
    		{
    			strcpy(response,"Good bye");
    		}
    		else if(strcmp("Hi",msg.mtext)==0)
    		{
    			strcpy(response,"Helloooo");
    		}
    		else
    		{
    			// Req: Need to creat separate path for choice 2 and 3
    			strcpy(response,"Middle choice");
    		}
    		
    		// Send messages to client via message queue
    		
    		strcpy(msg.mtext,response);
    		
    		if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1)
    		{
    			//Error in sending Message
    			perror("msgsnd");
    			exit(EXIT_FAILURE);
    		}
    	
    		printf("Message sent to client: %ld\n\n",msg.msgtype);
    		
    		exit(EXIT_SUCCESS);
    	}
    	else
    	{
    		// Maa Process
    		wait(NULL);
    	}
    	
    }
    
    /*********************************************************CLEANUP RELATED*************************************************************************/
    // Handle Remaining requests
    struct msqid_ds buf;
    
    // Get information about the message queue
    if (msgctl(msgqid, IPC_STAT, &buf) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    
    // Print the number of messages remaining in the queue
    printf("Number of messages in the queue: %ld\n", buf.msg_qnum);
    
    int cnt=buf.msg_qnum;
    while(cnt!=0)
    {
    	// Read whichever message is 1st in queue (kataar)
    	if(msgrcv(msgqid,&msg,MAX_MTEXT_SIZE,0,0)==-1)
    	{
    		perror("msgrcv");
    		exit(EXIT_FAILURE);
    	}
    	
    	clientID=msg.msgtype;
    	printf("Message received from client: %ld\n",msg.msgtype);
    	
    	// Check if cleanup message received
    	if(strcmp("-1",msg.mtext)==0)
    	{
    		//cleanup message received
    		printf("Terminating\n");
    		break;
    	}
    	
    	// Now that message is received, lets (drumrolls....) FORK IT
    	pid_t p=fork();
    	if(p==-1)
    	{
    		perror("fork");
    		exit(EXIT_FAILURE);
    	}
    	else if(p==0)
    	{
    		char response[100];
    		
    		// Bachha Process
    		
    		// Choice Handling
    		// Req: Need to create separate pipes for parent child communication
    		if(strcmp("4",msg.mtext)==0)
    		{
    			strcpy(response,"Good bye");
    		}
    		else if(strcmp("Hi",msg.mtext)==0)
    		{
    			strcpy(response,"Helloooo");
    		}
    		else
    		{
    			// Req: Need to creat separate path for choice 2 and 3
    			strcpy(response,"Middle choice");
    		}
    		
    		// Send messages to client via message queue
    		
    		strcpy(msg.mtext,response);
    		
    		if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1)
    		{
    			//Error in sending Message
    			perror("msgsnd");
    			exit(EXIT_FAILURE);
    		}
    	
    		printf("Message sent to client: %ld\n\n",msg.msgtype);
    		
    		exit(EXIT_SUCCESS);
    	}
    	else
    	{
    		// Maa Process
    		wait(NULL);
    		cnt--;
    	}
    }
    
    // Cleanup and exit
    msgctl(msgqid, IPC_RMID, NULL); // Remove the message queue
    printf("Client %ld exiting...\n", clientID);
    exit(EXIT_SUCCESS);

    return 0;
}
