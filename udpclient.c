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
	timeout.tv_sec = 8;
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
	int checkIP = inet_pton(AF_INET, ip, &(serveraddr.sin_addr));
	if (checkIP != 0)
	{
		printf("Invalid ip address.");
		return 0;
	}
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	
	socklen_t len = sizeof(serveraddr);

	printf("Enter file name: ");
	char filename[5000];
	fgets(filename,5000, stdin);
	sendto(sockfd,filename,strlen(filename)+1,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));

	// ack is the acknowledgement to be sent out, this will include
	// a byte saying which packet it received (position 0 - 9)
	char* ack = "Received Packet\n";
	char newFilename[100];
	filename[strlen(filename) - 1] = 0;
	strcpy(newFilename, "Received_");
	strcat(newFilename, filename);
	
	// opens output file
	FILE *f = fopen(newFilename, "w");
	if(f == NULL){
		printf("Could not open file to write\n");
		return(1);
	}
	// Infinite loop to receive packets, currently it just exits after 8 seconds
	// part 2 we will make the timeout much shorter and it will break and exit at end of file,
	// indicated probably by a special packet
	while(1){
		char packet[5000];
		// int n gets bytes from packet received
		int n = recvfrom(sockfd, packet, 5000, 0, (struct sockaddr*) &serveraddr, &len);
		if(n==-1){
			printf("TIMEOUT\n");
			break;
		} else {
			// if packet is received, write it, send ack
			printf("Received %i bytes, sending ack\n", n);
			fwrite(packet, 1, n, f);
			sendto(sockfd,ack,strlen(ack)+1,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
		}
	}
	
	close(sockfd);
	return(0);

}
