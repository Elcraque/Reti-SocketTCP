/*
 * protocol.h
 *
 *  Created on: 21 nov 2021
 *      Author: Antonello Mercadante Andrea Pezzuto
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define PORT 80 // port
#define QLEN 5 // request queue
#define BUFFERSIZE 1024 // receive buffer dim
#define ANSSIZE 80 // max dim for the answer to send to the client
#define OPSIZE 20 // max operand dim
#define MSGSIZE 43 // max message dim
#define TRUE 1 // boolean value
#define NO_ERROR 0 // WSASTARTUP Error flag

/////////////////////// functions definitions ///////////////////////

int add(int a,int b)
{
	return a+b;
}

int mult(int a,int b)
{
	return a*b;
}

int sub(int a,int b)
{
	return a-b;
}

float division(int a,int b)
{
	return (float) a/b;
}

const char* getfield(char* line, int num){     //This function takes a line, divides it, and returns the field indicated by the number num.
	char *linecopy = strdup(line);
	const char* tok;
	for (tok = strtok(linecopy, " ");
			tok && *tok;
			tok = strtok(NULL, " \0"))
	{
		if (!--num)
			return tok;
	}
	return NULL;
}

int isoperator (const char operator){
	int i=0;
	if (operator == '+' || operator == '*' || operator == '-' || operator == '/' || operator == '=' ){
		i=1;
	}
	return i;
}

int isoperand (char operand[]){
	int i = 0;
	int k = 0;
	while(operand[k] != '\0'){
		if (isdigit(operand[k])){
			i = 1;
		}
		k++;
	}
	return i;
}

int parselen(char toparse[]){
	int i = 0;
	int counter=0;
	while(toparse[i] != '\0'){
		if(toparse[i] == ' '){
			counter++;
		}
		i++;
	}
	return counter+1;
}

void clearwinsock()
{
#if defined WIN32
    WSACleanup();
#endif
}

void calculate(char *answermsg, const char *buf)
{
    // gets data from the received string
    char opcode = buf[0];

    // temporary buffers to convert the operands to integers
    char op1_str[OPSIZE] = "";
    char op2_str[OPSIZE] = "";

    int op1; // 1st operand
    int op2; // 2nd operand

    int i = 0;
    int j = 0;

    // gets the substring containing the 1st operand
    for (i = 0, j = 2; buf[j] != '|'; ++i, ++j) {
        op1_str[i] = buf[j];
    }

    // gets the substring containing the 2nd operand
    for (i = 0, j = j + 1; buf[j] != '\0'; ++i, ++j) {
        op2_str[i] = buf[j];
    }

    // converts the operand strings into integers
    op1 = strtol(op1_str, NULL, 0);
    op2 = strtol(op2_str, NULL, 0);

    char tmp[OPSIZE + 1] = "";

    switch(opcode) {
    case '+':
        strcpy(answermsg, "Addition result: ");
        snprintf(tmp, OPSIZE,"%d", add(op1,op2));
        strcat(answermsg, tmp);
        break;
    case '-':
        strcpy(answermsg, "Subtraction result: ");
        snprintf(tmp, OPSIZE,"%d", sub(op1,op2));
        strcat(answermsg, tmp);
        break;
    case '*':
        strcpy(answermsg, "Multiplication result: ");
        snprintf(tmp, OPSIZE,"%d", mult(op1,op2));
        strcat(answermsg, tmp);
        break;
    case '/':
        if (op2 != 0) {
            strcpy(answermsg, "Division result: ");
            snprintf(tmp, OPSIZE,"%.2f", division(op1,op2));
            strcat(answermsg, tmp);
        } else {
            strcpy(answermsg, "Error! attempt to divide by 0!");
        }
        break;
    case '=':
    	strcpy(answermsg, "END CLIENT PROCESS");
     	break;
    default:
        strcpy(answermsg, "END CLIENT PROCESS");  // If any error accurred, by default the server sends the closing string message
        break;
    }
}


#endif /* PROTOCOL_H_ */
