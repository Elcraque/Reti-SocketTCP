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

void calculate(char *answermsg, const char *buf){
    // gets operand from the received string in the 1st position
    char operand = buf[0];

    // temporary variables of operands converted into strings
    char operator1stTostr[OPSIZE] = "";
    char operator2stTostr[OPSIZE] = "";

    int operator1st; // 1st operand
    int operator2st; // 2nd operand

    int i = 0;
    int j = 0;

    // gets the 1st operand
    for (i = 0, j = 2; buf[j] != '|'; ++i, ++j) {
    	operator1stTostr[i] = buf[j];
    }

    // gets the 2nd operand
    for (i = 0, j = j + 1; buf[j] != '\0'; ++i, ++j) {
    	operator2stTostr[i] = buf[j];
    }

    // converts the operand strings to integers
    operator1st = strtol(operator1stTostr, NULL, 0);
    operator2st = strtol(operator2stTostr, NULL, 0);

    char tmp[OPSIZE + 1] = "";


    switch(operand) {
    case '+':
        strcpy(answermsg, "Addition result: ");
        snprintf(tmp, OPSIZE,"%d", add(operator1st,operator2st));  //convert integer in to string
        strcat(answermsg, tmp);
        break;
    case '-':
        strcpy(answermsg, "Subtraction result: ");
        snprintf(tmp, OPSIZE,"%d", sub(operator1st,operator2st));  //convert integer in to string
        strcat(answermsg, tmp);
        break;
    case '*':
        strcpy(answermsg, "Multiplication result: ");
        snprintf(tmp, OPSIZE,"%d", mult(operator1st,operator2st));  //convert integer in to string
        strcat(answermsg, tmp);
        break;
    case '/':
        if (operator2st != 0) {
            strcpy(answermsg, "Division result: ");
            snprintf(tmp, OPSIZE,"%.2f", division(operator1st,operator2st)); //convert integer in to string
            strcat(answermsg, tmp);
        } else {
            strcpy(answermsg, "Error! cannot divide by 0!");
        }
        break;
    case '=':
    	strcpy(answermsg, "CLOSING CLIENT CONNECTION");
     	break;
    default:
        strcpy(answermsg, "CLOSING CLIENT CONNECTION"); // If any error accurred, by default the server sends the closing string message
        break;
    }
}


#endif /* PROTOCOL_H_ */
