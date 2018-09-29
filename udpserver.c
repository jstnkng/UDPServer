#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char** argv){
	int sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	
	struct sockaddr_in serveraddr, clientaddr;
	serveraddr.sin_family=AF_INET;
	printf("Enter port number: ");
	char port[500];
	fgets(port,500,stdin);
	int p = atoi(port);
	serveraddr.sin_port=htons(p);
	serveraddr.sin_addr.s_addr=INADDR_ANY;

	bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	// Receiving file request
	socklen_t len = sizeof(clientaddr);
	int clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
	char line[500000];
	// receiving file request
	while(1){
		int n = recvfrom(sockfd, line, 5000, 0, (struct sockaddr*) &clientaddr, &len);
		if(n==-1){
			printf("timeout on receive\n");
		}else{
			break;
		}
	}
	char *fix = line;	
	// Removing trailing new line	
	fix[strlen(fix) - 1] = 0;	
	FILE *f = fopen(fix, "r");
	if(f == NULL){
		printf("fopen failed, errno = %d\n", errno);
		printf("No file found by that name\n");
	} else {
		// File found, begins transfer
		int packetSize = 1000;
		printf("File found, beginning transfer.\n");
		// Sending first 5 packets (window size 5)
		char packet[packetSize];
		// Transfers first 5 packets immediately with this four loop
		// this effectively is beginning the stream and setting the window to 5
		// since the client will be sending acks for each of these
		// Part 2 we will need a data structure to hold on to the "active" 5 packets
		// something along this lines of char* packets[10] to hold 10 spaces of possible packets
		// only 5 should be in there at a time, though
		for(int i = 0; i < 5; i++){
			int n = fread(packet, 1, packetSize - 1, f);
			if(n < 0){
				printf("Error reading from file\n");
				break;
			}
			if(n == 0){
				printf("End of file\n");
				break;
			} else {
				sendto(sockfd,packet,n,0,(struct sockaddr*) &clientaddr, sizeof(clientaddr));
				printf("Sent packet, %i bytes\n", n);
			}
		}
		// Infinite loop for sending packets until end of file
		while(1){
			int n = fread(packet, 1, packetSize - 1, f);
			if(n < 0){
				printf("Error reading from file\n");
				break;
			}
			if(n == 0){
				printf("End of file\n");
				break;
			} else { // else here means packet data available and successfully read
				char line[5000];
				// here if the ack is read by recvfrom, the next packet is sent
				// I think the fread above section should really be in the else statement here, otherwise
				// it keeps reading even if no ack is received for part 2, but other
				// things will change as well
				int r = recvfrom(sockfd, line, 5000, 0, (struct sockaddr*) &clientaddr, &len);
				if(r==-1){
					printf("waiting for ack\n");
				} else {
					printf("ack: %s\n", line);
					printf("Sent packet, %i bytes\n", n);			
					sendto(sockfd,packet,n,0,(struct sockaddr*) &clientaddr, sizeof(clientaddr));
				}
			}
		}
	}
}
