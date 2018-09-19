#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>

//CIS 457 Reliability Project
//Server.c
int main(int argc, char** argv)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	printf("Enter port number: ");
	int port;
	scanf("%d", &port);

	struct sockaddr_in serveraddr, clientaddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
		
	while(1)
	{
		int len = sizeof(clientaddr);
		char line[5000];
		recvfrom(sockfd, line, 5000, 0, (struct sockaddr*) &clientaddr, &len);
		sendto(sockfd, line, strlen(line),0,
			       (struct sockaddr*) &clientaddr, len);
		
		printf("Received from client: %s\n", line);
	}
	return 0;
}
