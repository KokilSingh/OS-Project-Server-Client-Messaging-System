// Including Relevant Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_MTEXT_SIZE 100

// Structure for Message Queue
struct message {
    long msgtype;		// Holds the Client ID
    int op;				// Holds the type of operation the client process wants.
    char mtext[100];	// Holds the file name on which the operation is intended.
};

int main() {

    // Declaring variables required for creation of message queue, and message struct.
    key_t key;
    int msgqid;
    struct message msg;

	// Variable which holds client choice.
    int choice; 

	// Getting the Message Queue ID of the Queue made by the server.
    key = ftok(".", 'b');

	// Show error if key is not created.
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	// Retrieving Message ID of message queue.
    msgqid = msgget(key, 0666 | IPC_CREAT);

	// Show error if message queue is not retrieved.
    if (msgqid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
	
    // Asking the process to enter it's ID.
    printf("Enter Client-ID:");
    scanf("%ld",&msg.msgtype);
    
    // Continuously listening for requests from client.
    while(1){

    	// Display Menu
    	printf("\nMenu:");
    	printf("Enter 1 to contact the Ping Server\n");
    	printf("Enter 2 to contact the File Search Server\n");
    	printf("Enter 3 to contact the File Word Count Server\n");
    	printf("Enter 4 if this Client wishes to exit\n");
    	printf("Enter choice:");
    	scanf("%d",&choice);
    	if (msgget(key, 0666) == -1)
    	{
    		printf("Server has been shutdown\n");
    		break;
    	}
    	
    	// If choice is 1, the client will send Hi, and the server will respond with Hello.
    	if(choice==1){
    		strcpy(msg.mtext,"Hi");
    	}

		// Asking for file name if choice is 2 or 3.
    	else if(choice==2||choice==3){
    		printf("Enter file name:");
    		scanf("%s",msg.mtext);	
    	}

		// Stopping the process if choice is 4.
    	else if(choice==4){
    		strcpy(msg.mtext,"4");
    	}

		// If the choice is something else, ignore and continue.
    	else{
    		continue;
    	}
    	
		// Setting the operation of message struct as the choice given by the client.
    	msg.op=choice;

    	// Send message to main server and checking if there's any error.
    	if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1){
			perror("msgsnd");
			exit(EXIT_FAILURE);
    	}
    	
    	printf("\nMessage sent to server\n");
    	
    	// Waiting reply from server with this client ID only and checking if there's any error.
    	if(msgrcv(msgqid,&msg,MAX_MTEXT_SIZE,msg.msgtype,0)==-1){
    		perror("msgrcv");
    		exit(EXIT_FAILURE);
    	}

		if(choice==2 || choice == 3)
		{
			if(strlen(msg.mtext)>1) 
			{	
				if(choice==2)
					printf("\nFile Present in current directory\n");
				else
					printf("\nServer Reply: %s\n",msg.mtext);
			}
			else printf("\nFile NOT Present in current directory\n");
		}
		else if(choice == 1)
		{
			printf("\nServer Reply: %s\n",msg.mtext);
		}
		// If the choice is 4, exit.
    	else if(choice==4){
    		break;
		}
    	
    }

	printf("Client Closing.....\n");

    return 0;
}
