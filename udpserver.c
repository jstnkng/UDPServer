
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

/* SERVER */
int main(int argc, char** argv){
	int sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	struct timeval timeout;
	timeout.tv_sec = 1;
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
	char line[5000];
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
		int headerSize = 1;
		printf("File found, beginning transfer.\n");
		// Sending first 5 packets (window size 5)
		char packet[packetSize+headerSize];
		char *packetsNA[10]; // Packets Not Acknowledged
		int sizes[10]; // Sizes of packets in packetsNA
		// Mallocing space for each NA packet
		for(int i = 0; i < 10; i++){
		  packetsNA[i] = malloc(packetSize+headerSize);
		}					  
		int acksRecv[10];
		for(int i = 0; i < 5; i++){
			int n = fread(packet + headerSize, 1, packetSize, f);
			packet[0] = i; // Tells position
			if(n < 0){
				printf("Error reading from file\n");
				break;
			}
			if(n == 0){
				printf("End of file\n");
				break;
			} else {
			  memcpy(packetsNA[i], packet, n+headerSize);
			  sizes[i] = n+headerSize;
				sendto(sockfd,packet,n+headerSize,0,(struct sockaddr*) &clientaddr, 				      sizeof(clientaddr));
				printf("Sent packet, %i bytes\n", n+headerSize);
			}
		}
		// Infinite loop for sending packets until end of file
		int packetNo = 5; // current packet no
		int currentPosition = 0; // current position to recv ack
		while(1){
				char ack[1];
				int r = recvfrom(sockfd, ack, 2, 0, (struct sockaddr*) &clientaddr, &len);
				if(r==-1){
					// Resending packet of interest if ack not received
					// TO DO fix size of need to hold on to packet sizes
					sendto(sockfd,packetsNA[currentPosition],sizes[currentPosition],0,	(struct sockaddr*) &clientaddr, sizeof(clientaddr));
					printf("waiting for ack, resending packet %i\n", (int) packetsNA[currentPosition][0]);
					printf("Should be resending: %i\n", currentPosition);
					//printf("Packet %s\n", packetsNA[currentPosition]+1); 
				} else {
					acksRecv[(int)ack[0]] = 1;
					printf("Ack:%i", (int)ack[0]);
					printf(" CurrPos:%i\n", currentPosition);
					if((int)ack[0] == currentPosition){
					        acksRecv[currentPosition] = 0;
						int n = fread(packet+headerSize, 1, packetSize, f);	
						packet[0] = packetNo; // Tells position
						if(n < 0){
							printf("Error reading from file\n");
							return(-1);
						}
						if(n == 0){
							printf("End of file\n");
							packet[0] = 'e';
							packet[1] = packetNo;
							sendto(sockfd,packet,2,0,(struct sockaddr*) &clientaddr, sizeof(clientaddr));
							memcpy(packetsNA[packetNo], packet, n+headerSize);
							packetNo++;
							currentPosition++;
						} else {
							printf("ack: %i\n", (int)ack[0]);
							printf("Sent packet, %i bytes\n", n+headerSize);
							printf("packetNo: %i, actual #: %i\n", packetNo, (int)packet[0]);
						        memcpy(packetsNA[packetNo], packet, n+headerSize);
							sizes[packetNo] = n+headerSize;
							//printf("packetsNA: %s\n", packetsNA[packetNo] + 1);
							printf("packetNo: %i, actual #: %i\n", packetNo, (int)packet[0]);
							sendto(sockfd,packet,n+headerSize,0,(struct sockaddr*) &clientaddr, 									sizeof(clientaddr));
							acksRecv[currentPosition] = 0;
							//packetsNA[currentPosition] = 0;
							packetNo++;
							currentPosition++;
							if(currentPosition == 10){ currentPosition = 0; }
							if(packetNo == 10){ packetNo = 0; }
							int i = 0;
							while(i < 4){
							  if(acksRecv[currentPosition]==1){
							    acksRecv[currentPosition] = 0;
							    //packetsNA[currentPosition] = 0;
							    currentPosition++;
							    if(currentPosition == 10){ currentPosition = 0; }
						
							  } else { break; }
							  i++;
							}
						}
						// says client has received all of file
					} else if ((char)ack[0] == 'c'){
					      break; 
					}
			}
		}
		fclose(f);
		close(sockfd);
	}
}


