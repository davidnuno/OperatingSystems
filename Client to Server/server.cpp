/*
DAVID NUNO
COSC 3360
1. A client program that will connect with your
server and send it a single message requesting a
priority number.

2. A server program that will wait for connection
requests from client processes and send them
back their priority number starting at one and
incremented by one at each new request.

server.cpp
*/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <vector>

using namespace std;

struct clientInfo{                                                                          //STRUCT TO STORE CLIENT INFO
    string name;                                                                            //STORE NAME
    int priority;                                                                           //STORE PRIORITY
};

int main() {

    string myArray[1024];
    int portNum = 5555; //default port
    int client, server;
    int sockBuff = 1024;
    char buffer[sockBuff];
    bool terminate = false;
    struct sockaddr_in server_addr;
    socklen_t size;
    string sub;
    bool isExit = false;

    clientInfo clientArr[10];
    int count = 0;

    cout << "Enter a port number: ";
    cin >> portNum;
    cout << endl;

    char myname[256];
    struct hostent *hp;
    string hostname;

    gethostname(myname, 256);                                                               //GET HOST NAME
    hp = gethostbyname(myname);                                                             //SET HP TO HOST NAME

    if (hp == NULL) {                                                                       //IF ERROR RETRIEVING HOST NAME
        cout << "Error retrieving host name...\n";
        exit(1);
    }//END IF

    hostname = hp->h_name;
    cout << "The host name is: " << hostname << endl << endl;

    if ((client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {                                   //IF ERROR ESTABLISHING CLIENT
        cout << "Error establishing socket...\n";
        cout << "Exiting..\n";
        exit(1);
    }//END IF

    cout << "Socket server has been initialized...\n";

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    if ((bind(client, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0) {          //IF ERROR BINDING
        cout << "Error binding connection, the socket has already been established...\n";
        cout << "Exiting...\n";
        return -1;
    }//END IF

    size = sizeof(server_addr);                                                             //SET SIZE OF SERVER ADDRESS
    cout << "Looking for clients...\n\n";

    listen(client, 65);                                                                     //LISTEN TO AT MOST 65

    do {                                                                                    //WHILE LOOP
        cout << "Waiting for socket connection...\n";                                       //WAIT FOR SOCKET CONNECTION

        server = accept(client, (struct sockaddr *)&server_addr, &size);                    //ESTABLISH CONNECTION

        if (server < 0)                                                                     //IF ERROR CREATING CONNECTION
            cout << "Error on accepting...\n" << endl;

        recv(server, buffer, sockBuff, 0);                                                  //RECEIVE DATA

        string dummy(buffer);                                                               //DUMMY STRING

        clientArr[count].name = dummy;                                                      //STORE NAME IN CLIENT ARRAY
        clientArr[count].priority = count + 1;                                              //STORE PRIORITY IN CLIENT ARRAY

        if(clientArr[count].name == "Terminate") {                                          //IF TERMINATE
            cout << "Server has read terminate.\n";
            break;
        }//END IF

        count++;                                                                            //INCREMENT COUNT BY 1
        cout <<">" << dummy << " has requested priority number.\n";

        strcpy(buffer, "\n");                                                               //SEND THIS JUST TO CONFIRM THAT CLIENT HAS RECEIVED DATA
        send(server, buffer, sockBuff, 0);                                                  //AND PROGRAM CAN CONTINUE


        close(server);
    } while(true);

    close(client);

    cout << "Name\t\tPriority\n";
    for(int i = 0; i < count; i++)
        cout << clientArr[i].name << "\t\t" << clientArr[i].priority << endl;
    cout <<"Done.";

    return 0;
}//END MAIN