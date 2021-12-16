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

int main(int argc, char **argv){

#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
#endif
	// SOCKET CREATION
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		puts("socket() failed");

	//
	if(argc != 2){
		puts("Error in input command line.IP and PORT will be setted automatically");
		// BUILDING SERVER ADDRESS
		struct sockaddr_in echoServAddr; // for binding server socket
		struct sockaddr_in echoClntAddr; // temporary incoming datagrams slot
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_port = htons(PORT);
		echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		// BIND SOCKET
		if ((bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0){
			puts("bind() failed");
		}
		unsigned int cliAddrLen;
		char buf[BUFFERSIZE];
		char answermsg[ANSSIZE + 1] = ""; // answer for the client
		int recvMsgSize;

		printf("Server Started on Port: %d \n",PORT);
		while(1) {
			// RECIVE DATA FROM CLIENT
			cliAddrLen = sizeof(echoClntAddr);
			memset(buf, 0, BUFFERSIZE); // cleans the buffer
			recvMsgSize = recvfrom(sock, buf, BUFFERSIZE, 0, (struct
					sockaddr*)&echoClntAddr, &cliAddrLen);
			// prints client's host name and IP
			struct hostent *host;
			if ((host = gethostbyaddr((char*) &echoClntAddr.sin_addr, 4, AF_INET)) == NULL) {
				puts("gethostbyaddr() failed.");
				return -1;
			}
			char *clientName = host->h_name;
			printf("Handling client %s \n", clientName);
			printf("Operation Requested: %s from Client: %s IP: %s \n", buf, clientName, inet_ntoa(echoClntAddr.sin_addr));

			// computes the result and builds a message for the client
			calculate(answermsg, buf);

			// SEND DATA TO CLIENT
			if (sendto(sock, answermsg, strlen(answermsg), 0, (struct sockaddr *)&echoClntAddr,
					sizeof(echoClntAddr)) != strlen(answermsg)){
				puts("sendto() sent different number of bytes than expected");
			}
		}
	}else{
		char inputStr[BUFFERSIZE];
		strcpy(inputStr,argv[1]);
		int i = 0;
		int j = 0;
		char serverName[BUFFERSIZE] = "";
		char thisPort[OPSIZE] = "";
		int port = 0;

		// gets the host
		for (i = 0, j = 0; inputStr[j] != ':'; ++i, ++j) {
			serverName[i] = inputStr[j];
		}
		printf ("SERVER NAME: %s \n", serverName);
		// gets the port
		for (i = 0, j = j + 1; inputStr[j] != '\0'; ++i, ++j) {
			thisPort[i] = inputStr[j];
		}
		port = atoi(thisPort);
		printf("PORT NUMBER: %d \n", port);

		struct hostent *host;

		if ((host = gethostbyname(serverName)) == NULL) {
			puts("gethostbyname() failed.");
			return -1;
		}

		struct in_addr *ina = (struct in_addr*) host->h_addr_list[0];
		printf("Server IP: %s\n\n", inet_ntoa(*ina));

		// BUILDING SERVER ADDRESS
		struct sockaddr_in echoServAddr; // for binding server socket
		struct sockaddr_in echoClntAddr; // temporary incoming datagrams slot
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_port = htons(port);
		//echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		echoServAddr.sin_addr.s_addr = ina->s_addr;

		// BIND SOCKET
		if ((bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
			puts("bind() failed");

		unsigned int cliAddrLen;
		char buf[BUFFERSIZE];
		char answermsg[ANSSIZE + 1] = ""; // answer for the client
		int recvMsgSize;

		printf("Server Started on Port: %d \n",port);
		while(1) {
			// RECIVE DATA FROM CLIENT
			cliAddrLen = sizeof(echoClntAddr);
			memset(buf, 0, BUFFERSIZE); // cleans the buffer
			recvMsgSize = recvfrom(sock, buf, BUFFERSIZE, 0, (struct
					sockaddr*)&echoClntAddr, &cliAddrLen);
			// prints client's host name and IP
			struct hostent *host;
			if ((host = gethostbyaddr((char*) &echoClntAddr.sin_addr, 4, AF_INET)) == NULL) {
				puts("gethostbyaddr() failed.");
				return -1;
			}
			char *clientName = host->h_name;
			printf("Handling client %s \n", clientName);
			printf("Operation Requested: %s from Client: %s IP: %s \n", buf, clientName, inet_ntoa(echoClntAddr.sin_addr));

			// computes the result and builds a message for the client
			calculate(answermsg, buf);

			// SEND DATA TO CLIENT
			if (sendto(sock, answermsg, strlen(answermsg), 0, (struct sockaddr *)&echoClntAddr,
					sizeof(echoClntAddr)) != strlen(answermsg))
				puts("sendto() sent different number of bytes than expected");
		}
	}

}

