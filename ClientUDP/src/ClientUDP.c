/* TCP socket client
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
        printf("error at WSAStartup\n");
        return -1;
    }
#endif

    char serverName[BUFFERSIZE] = ""; // server host name
    short port; // server port
    struct sockaddr_in fromAddr;
    unsigned int fromSize;
    char tmp[BUFFERSIZE] = ""; // buffer to receive a string
    char inputString[BUFFERSIZE] = "";
    int inputStringLen = 0;
    char *helloMsg = "Hi, I'm the client!";
    int msgLen;
    char echoBuffer[BUFFERSIZE] = "";
    int respStringLen;

    // asks the user for the server host name
    puts("Insert server name: ");

    fgets(tmp, BUFFERSIZE, stdin);
    sscanf(tmp, "%[^\n]", serverName);

    // asks the user for the server port number
    do {
        puts("Insert the server's port number: ");

        fgets(tmp, BUFFERSIZE, stdin);
        sscanf(tmp, "%hi", &port);

        if (port < 5000) {
            printf("Invalid port number!\nPort number must be greater than 5000\n");
        }

    } while (port < 5000);


    struct hostent *host;

    if ((host = gethostbyname(serverName)) == NULL) {
        fprintf(stderr,"gethostbyname() failed.\n");
        return -1;
    }

    struct in_addr *ina = (struct in_addr*) host->h_addr_list[0];
    printf("Server IP: %s\n\n", inet_ntoa(*ina));

    // creates an UDP socket
    int cSocket;
    cSocket = socket(PF_INET, SOCK_DGRAM, 0);

    if (cSocket < 0) {
    	fprintf(stderr,"Socket creation failed.\n");
        closesocket(cSocket);
        clearwinsock();
        return -1;
    }

    // builds the server address
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = ina->s_addr;
    //sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons(port);

    msgLen = strlen(helloMsg);
    // sends a message to the server
    if (sendto(cSocket, helloMsg, msgLen, 0, (struct sockaddr*) &sad, sizeof(sad)) != msgLen) {
    	fprintf(stderr,"sendto() sent different number of bytes than expected");
    }

    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(cSocket, echoBuffer, BUFFERSIZE, 0, (struct sockaddr*) &fromAddr, &fromSize);

    if (sad.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
    	fprintf(stderr,"Error: received a packet from unknown source.\n");
        closesocket(cSocket);
        clearwinsock();
        return -1;
    }

    echoBuffer[respStringLen] = '\0';
    printf("Received: %s\n", echoBuffer);

    char buf[BUFFERSIZE];
    char input_operation[MSGSIZE] = "";
        char operatorcode;// operation code (+ = Addition, - = Subtraction, * = Multiplication, / = Division, = 0 0 to quit)
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

        // sends the message
        int msglen = strlen(msg);
        // sends a message to the server
        if (sendto(cSocket, msg, msglen, 0, (struct sockaddr*) &sad, sizeof(sad)) != msglen) {
        	fprintf(stderr,"sendto() sent different number of bytes than expected");
        }

        //recive data from server
        respStringLen = recvfrom(cSocket, buf, BUFFERSIZE, 0, (struct sockaddr*) &fromAddr, &fromSize);
        buf[respStringLen] = '\0';

        // if recive CLOSING CLIENT CONNECTION, close the client else print the result
        puts("");
        if (strcmp(buf, "CLOSING CLIENT CONNECTION") == 0) {
        	puts("Closing Client Connection!");
        } else {
        	puts(buf);
        }

        // closes connection
        closesocket(cSocket);
        clearwinsock();

        printf("\nPress enter to quit-> ");
        getchar();

}
