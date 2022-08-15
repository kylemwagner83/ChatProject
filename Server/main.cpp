#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")


void main()
{
	// Initialize winsock
	WSADATA wsData;
	WORD ver MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "Can't initiate winsock! Quitting..." << std::endl;
		return;
	}


	// Create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket! Quitting..." << std::endl;
		return;
	}


	// Bind an ip address/port to the socket
	SOCKADDR_IN hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));


	// Listen on the socket
	listen(listening, SOMAXCONN);
	std::cout << "Waiting for client connection - Port: 54000, Connection Type: raw" << std::endl;


	// Create sets to manage multiple connections
	FD_SET master;
	FD_ZERO(&master);
	FD_SET(listening, &master);

	while (true)
	{
		FD_SET copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) // If a client connects to the listening socket, assume it is a new client
			{
				// Accept the new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the client list
				FD_SET(client, &master);

				// Send welcome message to the new client
				std::string welcomeMsg = "Welcome to the chat!\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				// Broadcast new connection to all clients
				for (int i = 0; i < master.fd_count; i++)
				{
					SOCKET outSock = master.fd_array[i];
					std::ostringstream ss;
					ss << "SOCKET #" << client << " has joined the chat!\r\n";
					std::string strOut = ss.str();
					send(outSock, strOut.c_str(), strOut.size() + 1, 0);
				}


			}
			else // If a client connects on another socket, assume it is sending a message
			{
				// Accept a new message
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Send message to all other clients but NOT the listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening ) // && outSock != sock
						{
							std::ostringstream ss;
							ss << "SOCKET #" << sock << ":" << buf;
							std::string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}


	// Clean up winsock
	WSACleanup();
	system("pause");

}