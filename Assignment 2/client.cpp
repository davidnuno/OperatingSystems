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

client.cpp
*/

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

int main()
{
    string hostGuess;
    int client;
    int sockBuff = 1024;
    int portNum = 5555; //default portNum
    int tryCnt = 0;
    char buffer[sockBuff];
    struct sockaddr_in server_addr;
    int count = 1;

    char myname[256];
    struct hostent *hp;
    string hostname;                                                                        //STORE HOST NAME

    gethostname(myname, 256);                                                               //GET HOST NAME
    hp = gethostbyname(myname);                                                             //SET HP TO HOST NAME
    if (hp == NULL) {                                                                       //IF ERROR RETRIEVING HOST
        cout << "\n> Error retrieving host name..." << endl;
        exit(1);
    }//END IF

    hostname = hp->h_name;                                                                  //GET HOST NAME

    cout << "> Enter the hostname (3 tries): ";                                             //PROMPT USER OF NUMBER OF ATTEMPTS
    cin >> hostGuess;                                                                       //ENTER HOST NAME
    cout << endl;

    while (hostGuess != hostname) {                                                         //WHILE LOOP TO ENTER HOST NAME
        if (tryCnt == 3) {                                                                  //IF COUNT ATTEMPTS EQUAL 3
            cout << "> Too many tries!\n";
            cout << "> Exiting...\n";
            exit(1);
        }//END IF
        cout << "Wrong host, enter the hostname again: \n";                                 //PROMPT USER TO TRY AGAIN
        cin >> hostGuess;
        tryCnt++;                                                                           //TRY COUNT INCREMENTS BY 1
    }//END WHILE LOOP

    cout << "Enter a port number: ";
    cin >> portNum;                                                                         //ENTER PORT NUMBER
    cout << endl;

    cout << "Enter 'Terminate' as a name to terminate the client.\n\n";

    do {
        client = socket(AF_INET, SOCK_STREAM, 0);

        if (client < 0) {                                                                   //IF ERROR ESTABLISHING CLIENT
            cout << "Error establishing socket...\n";
            exit(1);
        }//END IF

        cout << "\nA new socket has been created...\n\n";

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
        server_addr.sin_port = htons(portNum);

        if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {    //IF ERROR ESTABLISHING CONNECT
            cout << "> Did not connect";
            exit(1);
        }//END IF


        cout << "Enter name to receive priority: ";
        cin >> buffer;                                                                      //USER ENTERS NAME TO SEND
        string tempName(buffer);                                                            //STORE NAME IN TEMP VARIABLE

        send(client, buffer, sockBuff, 0);                                                  //SEND DATA

        if (tempName == "Terminate")                                                        //IF TERMINATE
            break;                                                                          //BREAK FROM WHILE LOOP

        recv(client, buffer, sockBuff, 0);                                                  //RECEIVE DATA
        
        cout << tempName << ", your priority number is: " << count <<endl;                  //PRINT DATA

        count++;                                                                            //INCREMENT COUNT BY 1
        close(client);                                                                      //CLOSE CLIENT

    } while (true);

    cout << "\nConnection terminated.\n";


    return 0;
}