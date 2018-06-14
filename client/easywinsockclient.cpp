//Daniel Pivalizza
//send a file to a server (only tested with txt files) via Winsock.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1"

class dealWithWebsocketsPrivately
{
private:
	WSADATA wsaData;
    SOCKET ConnectSocket;
    struct addrinfo *result,
                    *ptr,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen;
public:
	dealWithWebsocketsPrivately()//use this to initialize objects.
	{
		this->ConnectSocket= INVALID_SOCKET;
		this->result= NULL;
		this->ptr = NULL;
		this->recvbuflen=DEFAULT_BUFLEN;
	}
	int initializeWinsock()
	{
		this->iResult = WSAStartup(MAKEWORD(2,2), &this->wsaData);
		if (this->iResult != 0) 
			{
		    printf("WSAStartup failed with error: %d.\n", iResult);
		    system("PAUSE"); 
			return 1;
		}
		ZeroMemory( &this->hints, sizeof(this->hints) );
		this->hints.ai_family = AF_UNSPEC;
		this->hints.ai_socktype = SOCK_STREAM;
		this->hints.ai_protocol = IPPROTO_TCP;
		return 0;
	}
	int resolveServerNPort()
	{
		this->iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &this->hints, &this->result);
		if ( this->iResult != 0 ) 
		{
			printf("getaddrinfo failed with error: %d.\n", this->iResult);
		    WSACleanup();
			system("PAUSE"); 
			return 1;
		}
		return 0;
	}
	int connectToServerAddress()
	{
		for(this->ptr=this->result; this->ptr != NULL ;this->ptr=this->ptr->ai_next) 
		{
			// Create a SOCKET for connecting to server
			this->ConnectSocket = socket(this->ptr->ai_family, this->ptr->ai_socktype, this->ptr->ai_protocol);
			if (this->ConnectSocket == INVALID_SOCKET) 
			{
			    printf("socket failed with error: %ld.\n", WSAGetLastError());
			    this->socketCleanup();
			    system("PAUSE"); 
				return 1;
			}
			// Connect to server.
			this->iResult = connect( this->ConnectSocket, this->ptr->ai_addr, (int)this->ptr->ai_addrlen);
			if (this->iResult == SOCKET_ERROR) 
			{
			    closesocket(this->ConnectSocket);
			    ConnectSocket = INVALID_SOCKET;
			    continue;
			}
			break;
		}
		freeaddrinfo(this->result);
		if(this->ConnectSocket == INVALID_SOCKET) 
		{
			printf("Unable to connect to server!\n");
			this->socketCleanup();
			system("PAUSE"); 
			return 1;
		}
		return 0;	
	}
	void socketCleanup()//use to free memory and shutdown sockets.
	{
		closesocket(this->ConnectSocket);
		WSACleanup();
		return;
	}
	int sendData(char *fileBuffer, int fileSize)//for file buffer
	{
		this->iResult=send(this->ConnectSocket, fileBuffer, fileSize, 0);
		if(iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d.\n", WSAGetLastError());
			this->socketCleanup();
			system("PAUSE");
			return 1;
		}
		return 0;
	}
	int sendData(std::string fileBuffer, int fileSize)//for file buffer
	{
		this->iResult=send(this->ConnectSocket, fileBuffer.c_str(), fileSize, 0);
		if(iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d.\n", WSAGetLastError());
			this->socketCleanup();
			system("PAUSE");
			return 1;
		}
		return 0;
	}
	void recieveData()
	{
		// Receive until the peer closes the connection
		do 
		{
			this->iResult = recv(this->ConnectSocket, this->recvbuf, this->recvbuflen, 0);
			if ( this->iResult > 0 )
			    printf("Bytes received: %d.\n", this->iResult);
			else if ( this->iResult == 0 )
			    printf("Connection closed.\n");
			else
			    printf("recv failed with error: %d.\n", WSAGetLastError());
		}	
		while( this->iResult > 0 );
	}
	int shutdownSock()
	{
		this->iResult = shutdown(this->ConnectSocket, SD_SEND);
		if (this->iResult == SOCKET_ERROR) 
		{
			printf("shutdown failed with error: %d.\n", WSAGetLastError());
			this->socketCleanup();
			system("PAUSE"); 
			return 1;
		}
		return 0;
	}
};

int showMenu()
{
	int choice;
	printf("Please pick and option from the list below. \n1. Send a file.\n2. Send a file another way or maybe not because one of these is not going to work probably.\n3. Quit.\n");
	std::cin >> choice;
	return choice;
}

int main() 
{
	dealWithWebsocketsPrivately newSock=dealWithWebsocketsPrivately();
	std::string filename;
	std::fstream file;
    // Initialize Winsock
	if(newSock.initializeWinsock()==1)
	{
			return 1;
	}
    // Resolve the server address and port
    if(newSock.resolveServerNPort()==1)
		return 1;
    // Attempt to connect to a server address until one succeeds or quit if none succeed
	if(newSock.connectToServerAddress()==1)
		return 1;
	int choice=0;
	while(choice!=3)
	{
		choice=showMenu();
		if(choice==1)
		{
			std::ifstream inFile;
			//get filename from the user.
			std::cout << "What is the name of the file?" << std::endl;
			std::cin >> filename;
			//open the file
			inFile.open(filename, std::ios::in | std::ios::binary);
			if(!inFile.is_open() && !inFile.good())//if inFile isn't good, leave quickly.
			{
				std::cout << "File unfound, not good or not open... or all of them. Terminating program." << std::endl;
				newSock.socketCleanup();
				system("PAUSE");
				return 1;
			}
			//get the size of the file
			inFile.seekg(0, inFile.end);
			int fileSize=inFile.tellg();
			inFile.seekg(0, inFile.beg);
			//get the file in a buffer
			filename.append("\n");
			char* fileBuffer = new char[fileSize+filename.length()];
			for (int i=0; i<filename.length(); i++)//load filename into buffer
			{
				fileBuffer[i]=filename[i];
			}
			for (int i=filename.length(); i<fileSize+filename.length(); i++)//load file into buffer after filename is in it.
			{
				inFile.get(fileBuffer[i]);
			}
			inFile.close();
			for(unsigned int i=0;i<fileSize+filename.length()+1;i++)
			{
				std::cout << fileBuffer[i];
			}
			printf("\n");
			//send the file
			if(newSock.sendData(fileBuffer, fileSize+filename.length())==1)
			{
				newSock.socketCleanup();
				system("PAUSE");
				return 1;
			}
			delete [] fileBuffer;
		}
		if(choice==2)
		{
			//send this string as a test to verify that the server is getting the information properly.
			std::string letussee = "ACT I\nSCENE I. On a ship at sea: a tempestuous noise\nof thunder and lightning heard.\nEnter a Master and a Boatswain\nMaster\nBoatswain!\nBoatswain\nHere, master: what cheer?\nMaster\nGood, speak to the mariners: fall to't, yarely,\nor we run ourselves aground: bestir, bestir.\nExit";
			std::cout << "This is what we are about to send.\n" << letussee << std::endl;
			if(newSock.sendData(letussee.c_str(), letussee.length())==1)
				return 1;
		}
		if(choice==3)
		{
			printf("Okay, closing the program.\n");
		}
	}
    // shutdown the connection since no more data will be sent
	newSock.shutdownSock();
    // Receive until the peer closes the connection
	newSock.recieveData();
    // cleanup
	newSock.socketCleanup();
	system("PAUSE");
    return 0;
}