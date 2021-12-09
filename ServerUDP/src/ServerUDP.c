/* UDP socket server
 * Esonero reti 2021
 * Antonello Mercadante Andrea Pezzuto
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "protocol.h"

int main(void){
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
    	fprintf(stderr,"Error at WSAStartup()\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    char buf[BUFFERSIZE] = "";
    int recvMsgSize;
    char answermsg[ANSSIZE + 1] = ""; // answer for the client
    int str_len;


    char *okMsg = "OK";
    int msgLen;

    // creates an UDP socket
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    	fprintf(stderr,"socket() failed");
    	clearwinsock();
        return 0;
    }

    // builds server address
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family= AF_INET;
    echoServAddr.sin_port= htons(PORT);
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // binds the socket
    if ((bind(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr))) < 0) {
    	fprintf(stderr,"bind() failed");
    	closesocket(sock);
    	clearwinsock();
    	return 0;
    }

    puts("Server started.\n");

    // handles a client at a time
    while(1) {
    	memset(buf, 0, BUFFERSIZE); // initializes buffer
    	cliAddrLen = sizeof(echoClntAddr);
    	recvMsgSize = recvfrom(sock, buf, BUFFERSIZE, 0, (struct sockaddr*) &echoClntAddr, &cliAddrLen);

    	// prints client's host name and IP
    	struct hostent *host;
    	if ((host = gethostbyaddr((char*) &echoClntAddr.sin_addr, 4, AF_INET)) == NULL) {
    		fprintf(stderr,"gethostbyaddr() failed.\n");
    		return -1;
    	}

    	char *clientName = host->h_name;
    	puts("\nClient:");
    	printf("%s %s\n%s %s\n\n", "Name: ", clientName, "IP address: ", inet_ntoa(echoClntAddr.sin_addr));
    	printf("Client message: %s\n", buf);

    	// sends a confirmation message to the client
    	msgLen = strlen(okMsg);
    	if (sendto(sock, okMsg, msgLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr)) != msgLen) {
    		fprintf(stderr, "sendto() sent different number of bytes than expected");
    		closesocket(sock);
    		clearwinsock();
    		return 0;
    	}

    	// recive message from client
    	recvMsgSize = recvfrom(sock, buf, BUFFERSIZE, 0, (struct sockaddr*) &echoClntAddr, &cliAddrLen);

    	calculate(answermsg, buf);

    	// sends the message to the client
    	str_len = strlen(answermsg);
    	if (sendto(sock, answermsg, str_len, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr)) != str_len) {
    		fprintf(stderr,"sendto() sent different number of bytes than expected");
    		closesocket(sock);
    		clearwinsock();
    		return 0;
    	}
    }
}

