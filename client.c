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

	char line[5000];
	printf("Enter a filename: ");
 	scanf("%s", line);
	sendto(sockfd, line, strlen(line) + 1, 0, (struct sockaddr*) &serveraddr, sizeof(serveraddr));

	int len = sizeof(&serveraddr);
	recvfrom(sockfd, line, 5000, 0, (struct sockaddr*) &serveraddr, &len);

	printf("Received from server: %s\n", line);

	close(sockfd);

	return 0;
}
