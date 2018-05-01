// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread,coderead;
    int n;
    struct sockaddr_in serv_addr;
    char req_file[1024],loc_file[1024];
    char buffer[1024] = {0},fullpath[1024],flag[2]={0};
    FILE *fp;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
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
    printf("Enter the no of files required: ");
    scanf("%d",&n);
    while(n>0){
        getcwd(fullpath,1024);
        strcat(fullpath,"/");
        printf("Enter the name of required file: ");
        scanf("%s",req_file);
        printf("Enter the name of file to be created: ");
        scanf("%s",loc_file);
        if(send(sock , req_file , 1024 , 0 )==-1){  // send the message.
            perror("send error");
            return -1;
        }
        printf("File request sent\n");
        if(read(sock,flag,2)==-1){
            printf("Message not received\n");
            return -1;
        }
        if(flag[0]!='1'){
            printf("File not found\n");
        }
        else{
            strcat(fullpath,loc_file);
            fp = fopen(fullpath,"w");
            bzero(fullpath,1024); 
            if(fp==NULL)    printf("Could not create file\n");
            else{
                while(read( sock , buffer, 1024)!=-1){  // receive message back from server, into the buffer
                    if(buffer[strlen(buffer)-1]!='#')
                        printf("aa%ld\n",fwrite(buffer,1,strlen(buffer),fp));
                    else{
                        printf("bb%ld\n",fwrite(buffer,1,(strlen(buffer)-1),fp));
                        bzero(buffer,1024); 
                        break;
                    }
                    bzero(buffer,1024);                    
                }
                printf("File received succesfully\n");                
            } 
            fclose(fp);                   
        }
        bzero(loc_file,1024); 
        bzero(req_file,1024); 
        n--;
    }
    return 0;
}
