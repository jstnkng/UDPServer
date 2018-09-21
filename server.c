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

		// TODO:
		// Add packet receiving functionality

		FILE *f = fopen(line, "r");

		// Messages client if file cannot be found
		if (f == NULL)
		{
			char *message = "Could not find/open file";
			printf("Could not open file %s", line);
			sendto(sockfd, message, strlen(message), 0,
				   (struct sockaddr *)&clientaddr, len);
		}
		else
		{
			// Parses file contents
			fseek(f, 0, SEEK_END);
			long fileSize = ftell(f);
			fseek(f, 0, SEEK_SET);
			char *string = malloc(fileSize + 1);
			fread(string, fileSize, 1, f);
			fclose(f);
			sendto(sockfd, string, strlen(string)+1, 0,
				   (struct sockaddr *)&clientaddr, len);
		}
	}
	return 0;
}
