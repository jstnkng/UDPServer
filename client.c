#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char** argv){

	int sockfd = socket(AF_INET,SOCK_DGRAM, 0);
	if(sockfd < 0){
		printf("Error creating socket\n");
		return 1;
	}

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));


	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;

	// Asking for port number
	printf("Enter a port number: ");
	char port[500];
	fgets(port,50,stdin);
	int p = atoi(port);
	serveraddr.sin_port = htons(p); // host to network short

	// Asking for IP Address
	printf("Enter an IP address: ");
	char ip[500];
	fgets(ip,500,stdin);
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	
	socklen_t len = sizeof(serveraddr);

	printf("Enter file name: ");
	char filename[5000];
	fgets(filename,5000, stdin);
	sendto(sockfd,filename,strlen(filename)+1,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));

	char* ack = "Received Packet\n";

	FILE *f = fopen("output", "w");
	while(1){
		char packet[5000];
		int n = recvfrom(sockfd, packet, 5000, 0, (struct sockaddr*) &serveraddr, &len);
		if(n==-1){
			printf("TIMEOUT\n");
			break;
		} else {
			packet[n] = '\0';
			printf("Received %i bytes, sending ack\n", strlen(packet));
			fprintf(f, packet);
			sendto(sockfd,ack,strlen(ack)+1,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
		}
	}
	
	close(sockfd);
	return(0);

}
