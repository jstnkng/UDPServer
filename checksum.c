#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Reference: https://github.com/FRRouting/frr/blob/master/lib/checksum.c

u_short client_checksum(u_short *buf, int nbytes)
{
	register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*buf++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)buf;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

u_short server_checksum(u_short *buf, int nbytes, u_short checksum)
{
	register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*buf++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)buf;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)(sum);
    return ~(answer+checksum);
}



u_short checksumf(u_short *buf, int nbytes)
{
	register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
        sum+=*buf++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)buf;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)sum;

    return(answer);
}

// Get checksum value: ~result

// Get the checksum check: ~(result+checksum)


int main()
{
	u_short checksum;
    u_short checksum2;
	char buf[11];
	/* Zero out our string buffer */
	bzero(buf, sizeof(buf));

	/* Put a message in our string buffer */
	strcpy(buf, "hellohello");

	/* We must cast our buf to a u_short ptr for checksum */
	checksum = checksumf((u_short *)buf, 11);

    checksum = ~checksum; // Get checksum val to store

	/* Print checksum of client data */
	printf("Client checksum is 0x%x\n", checksum);

    checksum2 = checksumf((u_short *)buf, 11);
    checksum2 = ~(checksum+checksum2); // Check if value is not corrupted

    /* Find checksum using client checksum */
    // checksum = server_checksum((u_short*)buf, 11, checksum);
    
    /* Print checksum of server data and client data */
    printf("Server checksum using client checksum is 0x%x\n", checksum2);

    printf("Checksum value: %d\n",checksum);

//     char packet[30];
//     packet[0] = 1;
//     printf("%d\n", checksum);
//     packet[1] = (u_short)checksum;

//     u_short t = packet[1];
// printf("%d\n", t);

	return 0;
}