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


int main()
{
	u_short checksum;
	char buf[11];
	/* Zero out our string buffer */
	bzero(buf, sizeof(buf));

	/* Put a message in our string buffer */
	strcpy(buf, "hellohello");

	/* We must cast our buf to a u_short ptr for checksum */
	checksum = client_checksum((u_short *)buf, 11);

	/* Print checksum of client data */
	printf("Client checksum is 0x%x\n", checksum);

    /* Find checksum using client checksum */
    checksum = server_checksum((u_short*)buf, 11, checksum);
    
    /* Print checksum of server data and client data */
    printf("Server checksum using client checksum is 0x%x\n", checksum);
	return 0;
}