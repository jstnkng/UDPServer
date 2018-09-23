#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

//CIS 457 Reliability Project
//Client.c
int main(int argc, char** argv)
{
	printf("Enter server IP Address: ");
	char serverIP[20];
	fgets(serverIP, 20, stdin);
	printf("Enter port number: ");
	char port[10];
	fgets(port, 10, stdin);
	int portNumber;
	portNumber = atoi(port);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		printf("Error creating socket");
		return 1;
	}
	
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(portNumber);
	serveraddr.sin_addr.s_addr = inet_addr(serverIP);

	char filename[100];
	printf("Enter a filename: ");
 	scanf("%s", filename);
	sendto(sockfd, filename, strlen(filename) + 1, 0, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

	char newFilename[100];
	strcpy(newFilename, "Received_");
	strcat(newFilename, filename);

	// Reference: https://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files

	FILE *fp;
    fp = fopen(newFilename, "w");
    if(NULL == fp)
    {
        printf("Error creating file");
        return 1;
    }

	int len = sizeof(&serveraddr);
	int bytesReceived = 0;
	char recvBuff[1024];

	 /* 
	  * TODO: Add a header to data with information like size & seq to determine when to stop
	  * receiving packets. 
	  * Receives 1024 or less bytes of data at a time.
	  * Currently stops receiving data when server sends 1 byte of data to indicte the end of file.
	  */
    while((bytesReceived = recvfrom(sockfd, recvBuff, 1024, 0, (struct sockaddr*) &serveraddr, &len)) > 1)
    {
        printf("Bytes received %d\n",bytesReceived);    
        fwrite(recvBuff, 1,bytesReceived,fp);
    }

    if(bytesReceived < 0)
    {
        printf("\n Error receiving data \n");
    }

	close(sockfd);

	return 0;
}
