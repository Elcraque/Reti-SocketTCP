/* UDP socket client
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
	//int port;
	//char* host;
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
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		puts("socket() failed");
	}
	//
	if(argc != 2){
		puts("Error in input command line.IP and PORT will be setted automatically");
		// BUILDING SERVER ADDRESS
		struct sockaddr_in echoServAddr;
		struct sockaddr_in fromAddr;
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = PF_INET;
		echoServAddr.sin_port = htons(PORT);
		echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		char operatorcode;// operation code (+ = Addition, - = Subtraction, * = Multiplication, / = Division, = 0 0 to quit)
		do{
			// READING OPERATION TO DO
			char input_operation[MSGSIZE] = "";
			//char operatorcode;// operation code (+ = Addition, - = Subtraction, * = Multiplication, / = Division, = 0 0 to quit)
			int firstop; // 1st operand
			int secondop; // 2nd operand
			// temporary flags variable
			char op1_flag[OPSIZE] = "";
			char op2_flag[OPSIZE] = "";

			// temporary buffers of converted operands into strings
			char op1Tostr[OPSIZE] = "";
			char op2Tostr[OPSIZE] = "";

			int parse=0; // argument number in the inputstring
			char msg[MSGSIZE + 1] = ""; // message for the server

			do{
				parse=0; // parse reset

				printf ("PRESS:= 0 0 TO EXIT PROGRAM\nINSERT OPERATION TU DO (ES :+ 23 5):\t");
				fgets(input_operation, MSGSIZE, stdin);
				parse = parselen(input_operation); // check the argument number in the input string

				if(parse != 3){
					puts("Error!Argument number don t match");
				}else{

					operatorcode = input_operation[0];
					firstop = atoi(getfield(input_operation,2));
					snprintf(op1Tostr, OPSIZE, "%d", firstop); //printing the 1st operator in an array of chars
					secondop = atoi(getfield(input_operation,3));
					snprintf(op2Tostr, OPSIZE, "%d", secondop); //printing the 2nd operator in an array of chars
					strcpy(op1_flag,getfield(input_operation,2));
					strcpy(op2_flag,getfield(input_operation,3));

					if(isoperator(operatorcode) != TRUE){
						puts("Error! Invalid operator");
					}
					if(isoperand(op1_flag) != TRUE || isoperand(op2_flag) != TRUE){
						puts("Error! Invalid operand");
					}
				}

			}while(isoperator(operatorcode) != TRUE || isoperand(op1_flag) != TRUE || isoperand(op2_flag) != TRUE ||  parse != 3);

			// builds the message for the server
			//operator message buildig
			msg[0] = operatorcode; // saves the operation code
			msg[1] = '|'; // adding separator character
			//1st operand message building
			strcat(msg, op1Tostr); // copies the 1st operand
			msg[strlen(msg)] = '|'; // adding separator character
			//2nd operand message building
			strcat(msg, op2Tostr); // copies the 2nd operand
			msg[strlen(msg)] = '\0'; // adding the termination character
			// SEND STRING TO THE SERVER
			if (sendto(sock, msg, strlen(msg), 0, (struct
					sockaddr*)&echoServAddr, sizeof(echoServAddr)) !=
							strlen(msg)){
				puts("sendto() sent different number of bytes than expected");
			}
			unsigned int fromSize;
			fromSize = sizeof(fromAddr);
			int bytesRcvd = 0;
			char buf[BUFFERSIZE];
			memset(buf, 0, BUFFERSIZE); // cleans the buffer
			bytesRcvd = recvfrom(sock, buf, BUFFERSIZE, 0, (struct
					sockaddr*)&fromAddr, &fromSize);

			 struct hostent *sHost;
			 if ((sHost = gethostbyaddr((char*) &fromAddr.sin_addr, 4, AF_INET)) == NULL) {
				 puts("gethostbyaddr() failed.");
				 return -1;
			 }
			 char *srvName = sHost->h_name;

			if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
			{
				fprintf(stderr, "Error: received a packet from unknown source.\n");
				exit(EXIT_FAILURE);
			}
			if(bytesRcvd > 0){
				printf("Recived data from server %s IP %s:%d \n", srvName , inet_ntoa(echoServAddr.sin_addr), PORT);
				buf[bytesRcvd] = '\0';
				printf("%s\n", buf);
			}else{
				puts("Nothing was recived!Maybe server is on another port!");
			}
		}while(operatorcode != '=');
		closesocket(sock);
		clearwinsock();
		system("pause");
		return EXIT_SUCCESS;
	} else{
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
		struct sockaddr_in echoServAddr;
		struct sockaddr_in fromAddr;
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = PF_INET;
		echoServAddr.sin_port = htons(port);
		echoServAddr.sin_addr.s_addr = ina->s_addr;
		//echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		char operatorcode;// operation code (+ = Addition, - = Subtraction, * = Multiplication, / = Division, = 0 0 to quit)
		do{
			// READING OPERATION TO DO
			char input_operation[MSGSIZE] = "";
			//char operatorcode;// operation code (+ = Addition, - = Subtraction, * = Multiplication, / = Division, = 0 0 to quit)
			int firstop; // 1st operand
			int secondop; // 2nd operand
			// temporary flags variable
			char op1_flag[OPSIZE] = "";
			char op2_flag[OPSIZE] = "";

			// temporary buffers of converted operands into strings
			char op1Tostr[OPSIZE] = "";
			char op2Tostr[OPSIZE] = "";

			int parse=0; // argument number in the inputstring
			char msg[MSGSIZE + 1] = ""; // message for the server

			do{
				parse=0; // parse reset

				printf ("PRESS:= 0 0 TO EXIT PROGRAM\nINSERT OPERATION TU DO (ES :+ 23 5):\t");
				fgets(input_operation, MSGSIZE, stdin);
				parse = parselen(input_operation); // check the argument number in the input string

				if(parse != 3){
					puts("Error!Argument number don t match");
				}else{

					operatorcode = input_operation[0];
					firstop = atoi(getfield(input_operation,2));
					snprintf(op1Tostr, OPSIZE, "%d", firstop); //printing the 1st operator in an array of chars
					secondop = atoi(getfield(input_operation,3));
					snprintf(op2Tostr, OPSIZE, "%d", secondop); //printing the 2nd operator in an array of chars
					strcpy(op1_flag,getfield(input_operation,2));
					strcpy(op2_flag,getfield(input_operation,3));

					if(isoperator(operatorcode) != TRUE){
						puts("Error! Invalid operator");
					}
					if(isoperand(op1_flag) != TRUE || isoperand(op2_flag) != TRUE){
						puts("Error! Invalid operand");
					}
				}

			}while(isoperator(operatorcode) != TRUE || isoperand(op1_flag) != TRUE || isoperand(op2_flag) != TRUE ||  parse != 3);

			// builds the message for the server
			//operator message buildig
			msg[0] = operatorcode; // saves the operation code
			msg[1] = '|'; // adding separator character
			//1st operand message building
			strcat(msg, op1Tostr); // copies the 1st operand
			msg[strlen(msg)] = '|'; // adding separator character
			//2nd operand message building
			strcat(msg, op2Tostr); // copies the 2nd operand
			msg[strlen(msg)] = '\0'; // adding the termination character
			// SEND STRING TO THE SERVER
			if (sendto(sock, msg, strlen(msg), 0, (struct
					sockaddr*)&echoServAddr, sizeof(echoServAddr)) !=
							strlen(msg)){
				puts("sendto() sent different number of bytes than expected");
			}
			unsigned int fromSize;
			fromSize = sizeof(fromAddr);
			int bytesRcvd = 0;
			char buf[BUFFERSIZE];
			memset(buf, 0, BUFFERSIZE); // cleans the buffer
			bytesRcvd = recvfrom(sock, buf, BUFFERSIZE, 0, (struct
					sockaddr*)&fromAddr, &fromSize);
			if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
			{
				fprintf(stderr, "Error: received a packet from unknown source.\n");
				exit(EXIT_FAILURE);
			}
			if(bytesRcvd > 0){
				printf("Recived data from server %s IP %s:%d \n", serverName , inet_ntoa(echoServAddr.sin_addr), port);
				buf[bytesRcvd] = '\0';
				printf("%s\n", buf);
			}else{
				puts("Nothing was recived!Maybe server is on another port! \nPress = 0 0 to exit client! \n");
			}
		}while(operatorcode != '=');
		closesocket(sock);
		clearwinsock();
		system("pause");
		return EXIT_SUCCESS;
	}

}
