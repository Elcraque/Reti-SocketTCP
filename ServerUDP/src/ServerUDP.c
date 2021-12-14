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
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
#endif
	// CREAZIONE DELLA SOCKET
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		puts("socket() failed");

	// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	struct sockaddr_in echoServAddr; // for binding server socket
	struct sockaddr_in echoClntAddr; // temporary incoming datagrams slot
	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(PORT);
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// BIND DELLA SOCKET
	if ((bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
		puts("bind() failed");

	unsigned int cliAddrLen;
	char buf[BUFFERSIZE];
	char answermsg[ANSSIZE + 1] = ""; // answer for the client
	int recvMsgSize;

	puts("Server Started!");
	while(1) {
			cliAddrLen = sizeof(echoClntAddr);
			recvMsgSize = recvfrom(sock, buf, BUFFERSIZE, 0, (struct
					sockaddr*)&echoClntAddr, &cliAddrLen);
			printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
			printf("Operation Requested: %s from Cient: %s \n", buf, inet_ntoa(echoClntAddr.sin_addr));

			// computes the result and builds a message for the client
			        calculate(answermsg, buf);

			// INVIA LA STRINGA  AL CLIENT
			if (sendto(sock, answermsg, strlen(answermsg), 0, (struct sockaddr *)&echoClntAddr,
					sizeof(echoClntAddr)) != strlen(answermsg))
				puts("sendto() sent different number of bytes than expected");
		}
}

