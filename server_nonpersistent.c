#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    char* fname="Enter the File name:";
    char* morefiles = "Do you want to request more files: (y/n)"; 
    char fullpath[1024];
    getcwd(fullpath,1024);
    strcat(fullpath,"/Data/");
    int addrlen = sizeof(address);
    char buffer[1024] = {0},fname_buffer[1024]={0}, response_buffer[1024]={0};
    // char *hello = "Hello from server";
    int iter=1;
    // Creating socket file descriptor
    while(1)
    {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // This is to lose the pesky "Address already in use" error message
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                      &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
        address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
        address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *)&address,
                                     sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        // Port bind is done. You want to wait for incoming connections and handle them in some way.
        // The process is two step: first you listen(), then you accept()
        if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        int check=0;
        while(!check)
        {
        // sin_size = sizeof(struct sockaddr_in);

        // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        else
            printf("Connection established with the client.\n");
        send(new_socket , fname, strlen(fname) , 0 );  // send the file name prompt.
        valread = read(new_socket, fname_buffer, 1024); // read the file name given by the client.
        strcat(fullpath, fname_buffer);
        // char* fsend_name="/home/apoorav/Documents/compnet/ass1/file1.txt";
        send(new_socket , morefiles, strlen(morefiles) , 0 );  // send the file name prompt.
        valread = read(new_socket, response_buffer, 1024);
        printf("%s\n",response_buffer);
        char sendbuffer[1024]={0};
        FILE* fs1 = fopen(fullpath, "r");  // open the file requested by client for reading.
        if(fs1 == NULL)
        {
            fprintf(stderr,"ERROR: File %s not found on server.\n",fname_buffer);
            exit(EXIT_FAILURE);
        }
        bzero(sendbuffer, 1024);  // reset all the values of buffer.
        int fs1_block_sz,flag=0;
        while((fs1_block_sz = fread(sendbuffer, sizeof(char), 1024, fs1))>0)
        {
            if(send(new_socket, sendbuffer, fs1_block_sz, 0 )< 0)
            {
                fprintf(stderr, "Sending the file %s failed", fname_buffer);
                exit(EXIT_FAILURE);
            }
            // printf("\nEntered the while loop\n");
            // flag=1;
            bzero(sendbuffer, 1024);  // reset all the values of buffer.
        }
        printf("File sent to the client successfully\n");
        bzero(fname_buffer, 1024);
        bzero(response_buffer, 1024);
        check=1;
        fclose(fs1);
        }
        close(server_fd);
        // if(!strcmp(response_buffer,"n"))
        //     break;
    }
    return 0;
}
