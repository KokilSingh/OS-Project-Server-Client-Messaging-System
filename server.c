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


int main() {
    key_t key;
    int msgqid;
    key = ftok(".",'a');
    msgqid = msgget(key, 0664 | IPC_CREAT);

    int pipe_fd[2]; // Array to hold file descriptors for the pipe


    while(true){


        struct message nig; // Buffer to hold data
        msgrcv(msgqid,&nig,sizeof(nig),0,0);

        // Create the pipe
        if (pipe(pipe_fd) == -1) {
            perror("Pipe creation failed at line 11 in server.c\n");
            exit(-1);
        }

        // Create a child process (client)
        pid_t child_pid = fork();

        if (child_pid == -1) 
        {
            perror("Fork failed at line 19 in server.c");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) 
        {
            // Child process (client)
            close(pipe_fd[1]); // Close the write end of the pipe (not needed by the child)

            // Read data from the pipe
            //printf("Receiving message...\n");
            read(pipe_fd[0], &nig, sizeof(nig));
            if(nig.oprid == 1){
                strcpy(nig.mfile, "Hello");
                msgsnd(msgqid, &nig, sizeof(nig), 0);
            }

            close(pipe_fd[0]); // Close the read end of the pipe (client is done reading)


            /*  COPY PASTED TEMP.C*/
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
                const char *fileName = nig.mfile;
                // Replace the child process with a new process using execlp
                if(nig.oprid == 3){
                    if (execlp("wc","wc", "-w",fileName, NULL) == -1) {
                        perror("execlp failed");
                        exit(EXIT_FAILURE);
                    }
                }

                if(nig.oprid == 2){
                    if(execlp("find","find",".","-name",fileName,NULL) == -1){
                        perror("Find execlp failed");
                        exit(EXIT_FAILURE);
                    }
                }
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
                
                struct message send;
                if(nig.oprid == 3){
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
                    send.mtype = nig.mtype;
                    strcpy(send.mfile,buffer1);
                    send.oprid = 3;
                }

                else if(nig.oprid == 2){
                    send.mtype = nig.mtype;
                    strcpy(send.mfile,buffer);
                    printf("\n%s\n",buffer);
                    send.oprid = 2;
                }

                msgsnd(msgqid,&send,sizeof(send),0);
                //printf("Message sent\n"); 

                close(pipe_fd_1[0]); // Close the read end of the pipe (parent is done reading)
            }
        } 
        else 
        {
            // Parent process (server)
            close(pipe_fd[0]); // Close the read end of the pipe (not needed by the server)
            write(pipe_fd[1], &nig, sizeof(nig));
            //printf("Message sent nigg\n");
            close(pipe_fd[1]); // Close the write end of the pipe (server is done writing)
            wait(NULL);
        }
    }

    return 0;
}
