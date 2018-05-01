// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char fname_buffer[1024]={0}, response_buffer[1024]={0}, buffer[1024] = {0},buffer1[1024]={0};
    int iter=1;
    while(1)
    {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }
        // printf("%s eof\n",argv[1]);
        memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                    // which is meant to be, and rest is defined below

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Converts an IP address in numbers-and-dots notation into either a 
        // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        // send(sock , hello , strlen(hello) , 0 );  // send the message.
        // printf("Hello message sent\n");
        valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
        printf("%s\n",buffer );
        fgets(fname_buffer, 1024, stdin);  // get the file name input from the client.
        fname_buffer[strcspn(fname_buffer, "\n")] = 0;  // discard the trailing newline character in buffer.
        send(sock, fname_buffer, strlen(fname_buffer), 0);  // sened the file name requested.
        valread = read( sock , buffer1, 1024);  // receive message back from server, into the buffer
        printf("%s\n",buffer1 );  
        fgets(response_buffer, 1024, stdin);  // get the file name input from the client.
        response_buffer[strcspn(response_buffer, "\n")] = 0;  // discard the trailing newline character in buffer.
        send(sock, response_buffer, strlen(response_buffer), 0);  // sened the file name requested.
        // char* frcv_name="/home/apoorav/Documents/compnet/ass1/clientdir/file1.txt";
        FILE* fr1 = fopen(fname_buffer, "a");   // create the file entered by client to be written into. 
        if(fr1 == NULL)
        {
            printf("File %s cannot be opened on client side.\n", fname_buffer);
        }
        else
        {
            char rcvbuff[1024];  
            // bzero(rcvbuff, 1024);
            int fr_block_sz = 0;
            while((fr_block_sz=recv(sock, rcvbuff, 1024, 0))>0)  // read the data in server's directory's file.
            {
                int wrt = fwrite(rcvbuff, sizeof(char), fr_block_sz, fr1);  // write the received data into created file.
                if(wrt < fr_block_sz)
                {
                    error("File write failed on client side.\n");
                }
                bzero(rcvbuff, 1024);
                if(fr_block_sz == 0 || fr_block_sz != 1024)
                    break;
            }
            // if(fr_block_sz<0)
            // {
            //     if(erno==EAGAIN)
            //         printf("recv() timed out.\n");
            //     else 
            //         fprintf(stderr,"recv() failed due to errno = %d\n",errno);
            // }
            printf("Received file from the server.\n");
            fclose(fr1);    
            bzero(fname_buffer, 1024);
            bzero(response_buffer, 1024);

        }
        // if(!strcmp(response_buffer,'n'))
        //     break;
        close(sock);
    }
    return 0;
}
