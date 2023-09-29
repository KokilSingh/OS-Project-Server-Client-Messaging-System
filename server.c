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
    int op;
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
    
    /*****PIPES******/
    
    int pipe_fds[2]; // hold pipe file descriptors

    
	
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
    	
    	// Check if cleanup message received ********************* CLEAN UP MESSAGE CHECKING
    	if(strcmp("-1",msg.mtext)==0)
    	{
    		//cleanup message received
    		printf("Terminating\n");
    		break;
    	}
    	
    	// Create pipes for communication with child processes
    	if (pipe(pipe_fds) == -1) {
     	       perror("pipe");
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
    		struct message Text;
    		char response[100];
    		
    		// Child Process
    		
    		
    		
    		// Step 3: Close Write End of Pipe
    		close(pipe_fds[1]);
    		
    		// Step 4: Accept Message from pipe 
    		
    		
        	ssize_t bytes_read = read(pipe_fds[0], &Text, sizeof(Text));
        	if (bytes_read == -1) {
        	    perror("read");
        	    exit(1);
        	}

        	printf("Child received message type: %ld\n", Text.msgtype);
        	printf("Child received operation type: %d\n",Text.op);
        	printf("Child received message text: %s\n", Text.mtext);
        
    		
    		// Choice Handling
    		if(strcmp("4",Text.mtext)==0)
    		{
    			strcpy(response,"Good bye");
    		}
    		else if(strcmp("Hi",Text.mtext)==0)
    		{
    			strcpy(response,"Helloooo");
    		}
    		else 
    		{
    			// Req: Need to creat separate path for choice 2 and 3

    				
    				
    			/************************************* 2 and 3 handling **********************************************************/
    			
    			// We create another pipe
    			int pipe_fd_1[2];
            		pid_t child_pid_1;
            		if (pipe(pipe_fd_1) == -1) {
              		  perror("Pipe creation failed");
             		  exit(EXIT_FAILURE);
            		}

            		// Create a child process
            		child_pid_1 = fork();

            		if (child_pid_1 == -1) {
            		    perror("Fork failed");
            		    exit(EXIT_FAILURE);
            		}

            		if (child_pid_1 == 0) {   // This is the server->child->child process 
              		  	// Child process
              		  	close(pipe_fd_1[0]); // Close the read end of the pipe (not needed by the child)

             		  	// Redirect stdout to the write end of the pipe
             		  	 dup2(pipe_fd_1[1], 1);
              		  	 close(pipe_fd_1[1]); // Close the original write end of the pipe
             		  	 const char *fileName = Text.mtext;
             		  	 // Replace the child process with a new process using execlp
             		  	 if(Text.op == 3){
             		  	     if (execlp("wc","wc", "-w",fileName, NULL) == -1) {
              		   	       perror("execlp failed");
              		   	       exit(EXIT_FAILURE);
              		   	   }
              		  	 }

              		  	 if(Text.op == 2){
               		 	    if(execlp("find","find",".","-name",fileName,NULL) == -1){
               		 	        perror("Find execlp failed");
               		 	        exit(EXIT_FAILURE);
               		 	    }
               		  	 }
               		  	 
               		  	 exit(EXIT_SUCCESS);
            		} 
            
            		else {
               			 // This is the server->child process
               			 wait(NULL);
               			 close(pipe_fd_1[1]); // Close the write end of the pipe (not needed by the parent)
               			 char buffer[1024];
               			 ssize_t bytesRead;
               			 // Read data from the pipe (output of the child process)
               			 bytesRead = read(pipe_fd_1[0], buffer, sizeof(buffer));
               			 buffer[bytesRead] = '\0';
                
               			
               			 if(Text.op == 3){
               			     char buffer1[1024];
               			     int count = 0;
               			     int index = 0;
                		     for(int i=0;i<strlen(buffer);++i){
                   			     if(buffer[i] >= '0' && buffer[i] <= '9'){
                   			         buffer1[index] = buffer[i];
                   			         index++;
                    			    }
                   			     else{
                    			        break;
                     			   }
                   		     }
                   		 	buffer1[index] = '\0';
                    		 	
                   		 	strcpy(response,buffer1);
                   		 	
               		   	}

               			else if(Text.op == 2){
                	  	  
                	  	  strcpy(response,buffer);
                 	  	  printf("\n%s\n",buffer);
                 	   	
              		  	}

                		close(pipe_fd_1[0]); // Close the read end of the pipe (parent is done reading)
           		   }
            
            		/********************END OF 2 and 3 Handling ***********************/
    		}
    		
    		// Send messages to client via message queue
    		
    		strcpy(Text.mtext,response);
    		
    		if(msgsnd(msgqid,&Text,sizeof(Text.mtext),0)==-1)
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
    		// IF MESSAGE NOT FOR CLEAN UP
    	
    		// Step 1: Send message into pipe
    		if (write(pipe_fds[1], &msg, sizeof(msg)) == -1) {
        	    perror("write");
        	    exit(1);
        	}
    	
    		// Step 2: Close Read End of Pipe
    		close(pipe_fds[0]);
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
    	
    	
    	// Create pipes for communication with child processes
    	if (pipe(pipe_fds) == -1) {
     	       perror("pipe");
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
    		struct message Text;
    		char response[100];
    		
    		// Child Process
    		
    		
    		
    		// Step 3: Close Write End of Pipe
    		close(pipe_fds[1]);
    		
    		// Step 4: Accept Message from pipe 
    		
    		
        	ssize_t bytes_read = read(pipe_fds[0], &Text, sizeof(Text));
        	if (bytes_read == -1) {
        	    perror("read");
        	    exit(1);
        	}

        	printf("Child received message type: %ld\n", Text.msgtype);
        	printf("Child received operation type: %d\n",Text.op);
        	printf("Child received message text: %s\n", Text.mtext);
        
    		
    		// Choice Handling
    		if(strcmp("4",Text.mtext)==0)
    		{
    			strcpy(response,"Good bye");
    		}
    		else if(strcmp("Hi",Text.mtext)==0)
    		{
    			strcpy(response,"Helloooo");
    		}
    		else 
    		{
    			// Req: Need to creat separate path for choice 2 and 3

    				
    				
    			/************************************* 2 and 3 handling **********************************************************/
    			
    			// We create another pipe
    			int pipe_fd_1[2];
            		pid_t child_pid_1;
            		if (pipe(pipe_fd_1) == -1) {
              		  perror("Pipe creation failed");
             		  exit(EXIT_FAILURE);
            		}

            		// Create a child process
            		child_pid_1 = fork();

            		if (child_pid_1 == -1) {
            		    perror("Fork failed");
            		    exit(EXIT_FAILURE);
            		}

            		if (child_pid_1 == 0) {   // This is the server->child->child process 
              		  	// Child process
              		  	close(pipe_fd_1[0]); // Close the read end of the pipe (not needed by the child)

             		  	// Redirect stdout to the write end of the pipe
             		  	 dup2(pipe_fd_1[1], 1);
              		  	 close(pipe_fd_1[1]); // Close the original write end of the pipe
             		  	 const char *fileName = Text.mtext;
             		  	 // Replace the child process with a new process using execlp
             		  	 if(Text.op == 3){
             		  	     if (execlp("wc","wc", "-w",fileName, NULL) == -1) {
              		   	       perror("execlp failed");
              		   	       exit(EXIT_FAILURE);
              		   	   }
              		  	 }

              		  	 if(Text.op == 2){
               		 	    if(execlp("find","find",".","-name",fileName,NULL) == -1){
               		 	        perror("Find execlp failed");
               		 	        exit(EXIT_FAILURE);
               		 	    }
               		  	 }
               		  	 
               		  	 exit(EXIT_SUCCESS);
            		} 
            
            		else {
               			 // This is the server->child process
               			 wait(NULL);
               			 close(pipe_fd_1[1]); // Close the write end of the pipe (not needed by the parent)
               			 char buffer[1024];
               			 ssize_t bytesRead;
               			 // Read data from the pipe (output of the child process)
               			 bytesRead = read(pipe_fd_1[0], buffer, sizeof(buffer));
               			 buffer[bytesRead] = '\0';
                
               			
               			 if(Text.op == 3){
               			     char buffer1[1024];
               			     int count = 0;
               			     int index = 0;
                		     for(int i=0;i<strlen(buffer);++i){
                   			     if(buffer[i] >= '0' && buffer[i] <= '9'){
                   			         buffer1[index] = buffer[i];
                   			         index++;
                    			    }
                   			     else{
                    			        break;
                     			   }
                   		     }
                   		 	buffer1[index] = '\0';
                    		 	
                   		 	strcpy(response,buffer1);
                   		 	
               		   	}

               			else if(Text.op == 2){
                	  	  
                	  	  strcpy(response,buffer);
                 	  	  printf("\n%s\n",buffer);
                 	   	
              		  	}

                		close(pipe_fd_1[0]); // Close the read end of the pipe (parent is done reading)
           		   }
            
            		/********************END OF 2 and 3 Handling ***********************/
    		}
    		
    		// Send messages to client via message queue
    		
    		strcpy(Text.mtext,response);
    		
    		if(msgsnd(msgqid,&Text,sizeof(Text.mtext),0)==-1)
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
    		// IF MESSAGE NOT FOR CLEAN UP
    	
    		// Step 1: Send message into pipe
    		if (write(pipe_fds[1], &msg, sizeof(msg)) == -1) {
        	    perror("write");
        	    exit(1);
        	}
    	
    		// Step 2: Close Read End of Pipe
    		close(pipe_fds[0]);
    		wait(NULL);
    		cnt--;
    	}
    }
    
    // Cleanup and exit
    msgctl(msgqid, IPC_RMID, NULL); // Remove the message queue
    printf("Server %ld exiting...\n", clientID);
    exit(EXIT_SUCCESS);

    return 0;
}
