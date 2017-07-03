#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
/* for gethostbyname() */
#define PORT 123
/* server port the client connects to */
int main( int argc, char * argv[]) {
  int i;
  static char buf[48];
int sockfd, numbytes;
struct hostent *he;
 socklen_t addr_len;
struct sockaddr_in their_addr;
struct ntp_packet
{
  unsigned char li_vn_mode;
  unsigned char stratum;
  char poll;
  char precision;
};
struct ntp_packet myPacket;

/* server address info */
if( argc != 3) {
fprintf( stderr, "usage: talker hostname message\n");
exit( 1);
}
/* resolve server host name or IP address */
 //buf[0] = 0x23;
 myPacket.li_vn_mode = 0x23;
 myPacket.stratum = 4;
 myPacket.poll = 4;
 myPacket.precision = 4;
 memcpy(buf, &myPacket, sizeof(buf));
if( (he = gethostbyname( argv[ 1])) == NULL) {
perror( "Talker gethostbyname");
exit( 1);
}
if( (sockfd = socket( AF_INET, SOCK_DGRAM, 0)) == -1) {
perror( "Talker socket");
exit( 1);
}


memset( &their_addr,0, sizeof(their_addr));
/* zero struct */
their_addr.sin_family = AF_INET;
/* host byte order .. */
their_addr.sin_port = htons( PORT);
/* .. short, netwk byte order */
their_addr.sin_addr = *((struct in_addr *)he -> h_addr);
// send the ntp packet tp the server - here buf is a char array version of the ntp_packet constructed above
if( (numbytes = sendto( sockfd, buf, 48, 0,
(struct sockaddr *)&their_addr, sizeof( struct sockaddr))) == -1) {
perror( "Talker sendto");
exit( 1);
}
printf( "Sent %d bytes to %s\n", numbytes,
 inet_ntoa( their_addr.sin_addr));
// receive a char array from the server which represents the completed ntp_packet - need to convert the buf array back into an ntp_packet struct
if( (numbytes = recvfrom( sockfd, buf, 48 - 1, 0,
(struct sockaddr *)&their_addr, &addr_len)) == -1) {
perror( "Listener recvfrom");
exit( 1);
}
close( sockfd);
 for (i=0;i<48;i++)
   printf("%x",buf[i]);
return 0;
}
