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
#define PORT 5193 // port
#define QLEN 5 // request queue
#define BUFFERSIZE 1024 // receive buffer dim
#define ANSSIZE 80 // max size for the answer to send to the client
#define OPSIZE 20 // max operand dim
#define MSGSIZE 43 // max message dim

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

void clearwinsock()
{
#if defined WIN32
    WSACleanup();
#endif
}

void compute_result(char *answermsg, const char *buf)
{
    // gets data from the received string
    char opcode = buf[0];

    // temporary buffers to convert the operands to integers
    char op1_str[OPSIZE] = "";
    char op2_str[OPSIZE] = "";

    int op1;
    int op2;

    int i, j;

    // gets the substring containing the first operand
    for (i = 0, j = 2; buf[j] != '|'; ++i, ++j) {
        op1_str[i] = buf[j];
    }

    // gets the substring containing the second operand
    for (i = 0, j = j + 1; buf[j] != '\0'; ++i, ++j) {
        op2_str[i] = buf[j];
    }

    // converts the above strings to integers
    op1 = strtol(op1_str, NULL, 0);
    op2 = strtol(op2_str, NULL, 0);

    char tmp[OPSIZE + 1] = "";

    // computes the requested operation
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
            strcpy(answermsg, "Error: attempt to divide by 0!");
        }
        break;
    case '=':
    	strcpy(answermsg, "END CLIENT PROCESS");
     	break;
    default:
        strcpy(answermsg, "END CLIENT PROCESS");
        break;
    }
}


#endif /* PROTOCOL_H_ */
