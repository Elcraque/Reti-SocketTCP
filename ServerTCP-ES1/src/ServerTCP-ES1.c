/* TCP socket server
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

int main(void)
{
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
        fprintf(stderr, "Error in WSAStartup().\n");
        return -1;
    }
#endif

    // creates a TCP socket
    int server_socket;
    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        fprintf(stderr, "Socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    // assigns an address to the socket
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    sad.sin_port = htons(PORT);

    // binds IP address and port number to the socket
    if (bind(server_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        fprintf(stderr, "bind() failed.\n");
        closesocket(server_socket);
        clearwinsock();
        return -1;
    }

    // sets the socket to listen
    if (listen (server_socket, QLEN) < 0) {
        fprintf(stderr, "listen() failed.\n");
        closesocket(server_socket);
        clearwinsock();
        return -1;
    }

    // accepts a new connection request from a client
    struct sockaddr_in cad;
    int client_socket; // temporary socket
    int client_len;
    char *welcomemsg = "Connection established.";
    puts("Waiting for connection...");

    int bytes_rcvd = 0;
    char buf[BUFFERSIZE + 1];
    char answermsg[ANSSIZE + 1] = ""; // answer for the client
    int str_len;

    while(1) {
        memset(buf, 0, BUFFERSIZE); // initializes buffer
        client_len = sizeof(cad);

        if ((client_socket = accept(server_socket,
                (struct sockaddr*) &cad, &client_len)) < 0) {
            fprintf(stderr, "accept() failed.\n");
            closesocket(server_socket);
            clearwinsock();
            return 0;
        }
        printf("\nClient connected: %s\n", inet_ntoa(cad.sin_addr));

        // sends a confirmation message to the client
        str_len = strlen(welcomemsg);
        if(send(client_socket, welcomemsg, str_len, 0) != str_len) {
            fprintf(stderr, "send() sent a different number of bytes than expected.");
            closesocket(client_socket);
            clearwinsock();
            return -1;
        }

        // receives a string from the client
        if ((bytes_rcvd = recv(client_socket, buf, BUFFERSIZE, 0)) <= 0) {
            fprintf(stderr, "recv() failed or connection terminated.");
            closesocket(client_socket);
            clearwinsock();
            return -1;
        }

        // computes the result and builds a message for the client
        compute_result(answermsg, buf);

        // sends the message to the client
        str_len = strlen(answermsg);
        if(send(client_socket, answermsg, str_len, 0) != str_len) {
            fprintf(stderr, "send() sent a different number of bytes than expected.");
            closesocket(client_socket);
            clearwinsock();
            return -1;
        }
        puts("");
    }

    return 0;
}

