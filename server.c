#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

//CIS 457 Reliability Project
//Server.c
int main(int argc, char **argv)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	printf("Enter port number: ");
	int port;
	scanf("%d", &port);

	struct sockaddr_in serveraddr, clientaddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

	while (1)
	{
		int len = sizeof(clientaddr);
		char line[5000];
		recvfrom(sockfd, line, 5000, 0, (struct sockaddr *)&clientaddr, &len);

		printf("Filename from client: %s\n", line);

		FILE *f = fopen(line, "r");

		if (f == NULL)
		{
			char *message = "Could not find/open file";
			printf("Could not open file %s", line);
			sendto(sockfd, message, strlen(message), 0,
				   (struct sockaddr *)&clientaddr, len);
		}
		else
		{ 
			while (1)
			{
				// TODO: Need to add header to this buffer and that will act as
				// our packet.
				// Buffer contains contents of file
				unsigned char buff[1024];
				int nread = fread(buff, 1, 1024, f);
				printf("Bytes read %d\n", nread);
				if (nread > 0)
				{
					printf("Sending \n");
					sendto(sockfd, buff, nread, 0, (struct sockaddr *)&clientaddr, len);
				}
				if (nread < 1024)
				{
					if (feof(f))
						printf("End of file\n");
						// TODO: Remove line below when header functionality is added.
						// Sends 1 byte of data to indicate all the packets have been sent.
						sendto(sockfd, buff, 1, 0, (struct sockaddr *)&clientaddr, len);
					if (ferror(f))
						printf("Error reading\n");
					break;
				}
			}
		}
	}
	return 0;
}
