// Including All Necessary Header Files
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

// Structure for Message Queue
struct message {
    long msgtype; 		// Holds the Client ID
    int op;		  		// Holds the type of operation the client process wants.
    char mtext[100];	// Holds the file name on which the operation is intended.
};

int main() {
    // Declaring variables required for retrieving of message queue, and message struct.
    key_t key;
    int msgqid;
    struct message msg;
    
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

	// Holds the Client Input.
	char choice;

    // Listen continuously for requests whether to end the server or not.
    while(1){
		
		printf("Do you want to shut down the main server. Enter Y for Yes or N for No:\n");
		scanf(" %c",&choice);
		
		// Checking if input is Yes or No.
		if(choice=='Y' || choice=='y'){
			break;
		}	
		else{
			continue;
		}
	}
		
	// Cleanup Initiation
	// Send message to main server with a special mtype and a special operation.
	// Creating a message struct to send via the message queue. 
	msg.msgtype=999;
	msg.op=0;
	strcpy(msg.mtext,"-1");

	//Error in sending Message
	if(msgsnd(msgqid,&msg,sizeof(msg.mtext),0)==-1){
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	
	// Print message depicting successful termination.
	printf("\nMessage sent to server... Gracefully Terminating\n");
    return 0;
}