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
	timeout.tv_sec = 1;
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

	// ack is the acknowledgement to be sent out, this will include
	// a byte saying which packet it received (position 0 - 9)
	
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
	// Infinite loop to receive packets
	int currentPosition = 0;
	// packets to be written	
	char* packets[10];
	int sizes[10]; // sizes of packets
	int reachedEOF = 0; // 1 for true
	int EOFPosition;
	for(int i = 0; i < 10; i++){
	   packets[i] = malloc(2000);
	   packets[i][0] = 'z';
	}
	while(1){
		char packet[5000];
		char ack[1];
		// int n gets bytes from packet received
		int n = recvfrom(sockfd, packet, 5000, 0, (struct sockaddr*) &serveraddr, &len);
		if(n==-1){
			printf("TIMEOUT\n");
		} else {
		        packets[currentPosition][0] = 'z';
			printf("Received %i bytes, sending ack\n", n);
			// Sends ack for packet # received
			ack[0] = packet[0];
			sendto(sockfd,ack,2,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
			printf("Packet:%i ", (int)packet[0]);
			printf(" CurrPos:%i\n", currentPosition);
			if((int)packet[0] == currentPosition){
				printf("WRITING\n");
			        packets[currentPosition][0] = 'z';
				fwrite(packet+1, 1, n-1, f);
				currentPosition++;
				if(currentPosition == 10){ currentPosition = 0; }
				// If packet[0] == e this means EOF
		        } else if((char)packet[0] == 'e'){
			  reachedEOF = 1;
			  EOFPosition = packet[1];
			} else {
			   memcpy(packets[(int)packet[0]], packet, n);
			   sizes[(int)packet[0]] = n;
			}	        
						
		}
		int i = 0;
		while(i < 4){		                    
		  if((char)packets[currentPosition][0] != 'z'){
		    printf("%c\n", (char)packets[currentPosition][0]);
			         printf("Before Saved: %i\n", (int)packets[currentPosition][0]);
				 fwrite(packets[currentPosition]+1, 1, sizes[currentPosition]-1, f);
			         packets[currentPosition][0] = 'z';
				 currentPosition++;
				if(currentPosition == 10){ currentPosition = 0; }
				// Reached EOF with all packets written
		       } else if(reachedEOF == 1 && currentPosition == EOFPosition) {
			        ack[0] = 'c';
			        sendto(sockfd,ack,2,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
			        while(1){
				  sendto(sockfd,ack,2,0,(struct sockaddr*) &serveraddr, sizeof(serveraddr));
				  int n = recvfrom(sockfd, packet, 5000, 0, (struct sockaddr*) &serveraddr, &len);
				  if(n==-1){
				    printf("TIMEOUT\n");
				    fclose(f);
				      close(sockfd);
				      return(0);				    
				  }
				}
			} else { break; }
			i++;
	       }
	}
	fclose(f);
	close(sockfd);
	return(0);

}

