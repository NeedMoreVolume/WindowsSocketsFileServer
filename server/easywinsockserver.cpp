//Daniel Pivalizza
//Project: serverside file saving magic via Winsock

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1"

int main() 
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char *recvbuf=new char[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
	{
        printf("WSAStartup failed with error: %d \n", iResult);
		system("PAUSE"); 
        return 1;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) 
	{
        printf("getaddrinfo failed with error: %d \n", iResult);
        WSACleanup();
		system("PAUSE");
        return 1;
    }
    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) 
	{
        printf("socket failed with error: %ld \n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
		system("PAUSE");
        return 1;
    }
    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
	{
        printf("bind failed with error: %d \n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
		system("PAUSE");
        return 1;
    }
    freeaddrinfo(result);
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) 
	{
        printf("listen failed with error: %d \n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
		system("PAUSE");
        return 1;
    }
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) 
	{
        printf("accept failed with error: %d \n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
		system("PAUSE");
        return 1;
	}
    // No longer need server socket
    closesocket(ListenSocket);
    // Receive until the peer shuts down the connection
	std::fstream file;
	int flag=0;
    do 
	{
		if(flag==0)//only do this once.
		{
			std::string filename="newEntry.txt";//default name for new entries, will be changed later to the first line of the file, granted that the file is a text type file.
			file.open(filename, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out );
			if(!file.is_open())
			{
				printf("File unable to be opened.\n");
				system("PAUSE");
				// cleanup
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
		}
		iResult= recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) 
		{
            printf("Bytes received: %d \n", iResult);
			for(int i=0;i<iResult;i++)
			{
				std::cout << recvbuf[i];
				file << recvbuf[i];
			}
			std::cout << std::endl;
        }
        else if (iResult == 0)
            printf("Connection closing... \n");
        else  
		{
            printf("recv failed with error: %d \n", WSAGetLastError());
            closesocket(ClientSocket);
			WSAGetLastError();
            WSACleanup();
			system("PAUSE");
            return 1;
        }
		
		flag++;
    } 
	while (iResult > 0);
	file.close();
	// shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) 
	{
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
		system("PAUSE");
        return 1;
    }
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
	//make sure you resolve the filename to the correct name listed on the first line of the file.
	std::ifstream inFile;
	std::fstream dummyFile;
	std::string realFileName;
	//open the file
	inFile.open("newEntry.txt", std::ios::in | std::ios::binary);
	if(!inFile.is_open() && !inFile.good())//if inFile isn't good, leave quickly.
	{
		std::cout << "File unfound, not good or not open... or all of them. Terminating program." << std::endl;
		system("PAUSE");
		return 1;
	}
	//get the size of the file
	inFile.seekg(0, inFile.end);
	int fileSize=inFile.tellg();
	inFile.seekg(0, inFile.beg);
	char* fileBuffer = new char[fileSize];
	//load file into buffer
	inFile.read(fileBuffer,fileSize);
	inFile.seekg(0, inFile.beg);
	//get filename from first line.
	getline(inFile, realFileName);
	inFile.close();
	std::cout << realFileName << " is the name of the file." << std::endl;
	dummyFile.open(realFileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if(!dummyFile.is_open())
	{
		printf("File unable to be opened.\n");
		system("PAUSE");
		return 1;
	}
	for(int i=realFileName.length(); i<fileSize; i++)
	{
		dummyFile << fileBuffer[i];
	}
	//cleanup
	delete fileBuffer;
	dummyFile.close();
	delete recvbuf;
	system("PAUSE");
    return 0;
}