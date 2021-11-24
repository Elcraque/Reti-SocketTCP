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
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (result != NO_ERROR) {
        fprintf(stderr, "Error in WSAStartup().\n");
        return -1;
    }
#endif

    // creates a TCP socket
    int c_socket;
    c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (c_socket < 0) {
        fprintf(stderr, "Socket creation failed.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    // builds server address
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    sad.sin_port = htons(PORT);

    // asks the server for connection
    if (connect(c_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        fprintf(stderr, "connect() failed.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }


    // receives data from the server
    int bytesRcvd = 0;
    char buf[BUFFERSIZE];

    if ((bytesRcvd = recv(c_socket, buf, BUFFERSIZE - 1, 0)) <= 0) {
        fprintf(stderr, "recv() failed or connection terminated.");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }
    buf[bytesRcvd] = '\0';
    printf("%s\n\n", buf); // prints the message received from the server

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
    if (send(c_socket, msg, msglen, 0) != msglen) {
        fprintf(stderr, "send() sent a different number of bytes than expected.");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }


    // receives a string from the server
    if ((bytesRcvd = recv(c_socket, buf, BUFFERSIZE - 1, 0)) <= 0) {
        fprintf(stderr, "recv() failed or connection terminated.");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }
    buf[bytesRcvd] = '\0';

    // if recive CLOSING CLIENT CONNECTION, close the client else print the result
    puts("");
    if (strcmp(buf, "CLOSING CLIENT CONNECTION") == 0) {
        puts("Closing Client Connection!");
    } else {
        puts(buf);
    }

    // closes connection
    closesocket(c_socket);
    clearwinsock();

    printf("\nPress enter to quit-> ");
    getchar();

    return 0;
}
