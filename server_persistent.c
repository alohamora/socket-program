#include <stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
    FILE *fp;
    int addrlen = sizeof(address);
    char buffer_recv[1024] = {0},buffer_send[1024]={0};
    char fullpath[1024],flag[] = "1";
    char *temp = "#";
    // Creating socket file descriptor
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

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {   
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while(read( new_socket , buffer_recv, 1024)!=-1){  // read infromation received into the buffer
        getcwd(fullpath,1024);
        strcat(fullpath,"/Data/");
        strcat(fullpath,buffer_recv);
        printf("Requested file: %s\n",fullpath);
        fp = fopen(fullpath,"r");
        bzero(fullpath,1024);
        if(fp==NULL){
            flag[0] = '2';
            send(new_socket, flag , 2 , 0);
            printf("File transfer failed\n");
        }
        else{
            send(new_socket,flag,2,0);
            while(fread(buffer_send,sizeof(char),1024,fp)>0){
                send(new_socket , buffer_send , strlen(buffer_send) , 0 );
                bzero(buffer_send,1024);
            }  // use sendto() and recvfrom() for DGRAM
            send(new_socket,temp,strlen(temp),0);    
            printf("File transfer succesful\n");
        }
        bzero(buffer_recv,1024);
    }
    return 0;
}
