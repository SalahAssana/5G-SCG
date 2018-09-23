#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<cstdio>
#include<winsock2.h>
#include<fstream>
//#include<iostream>
#include<vector>
#include<complex>
#include<synchapi.h>
#include<io.h>



#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 2048				//Max length of buffer
#define ADDR "192.168.33.42"	//Address of adapter used for communication
#define PORT 4098				//Port to listen on for incoming data
#define boardid 0
#define packetsize 1456   // to reduce time expense, use heuristic value


int main()
{
	SOCKET s;
	struct sockaddr_in server;
	int recv_len, seqnum_prev, seqnum_new;
	//int bytenum_prev, bytenum_new;
	char udpbuf[BUFLEN];
	WSADATA wsa;
	//int bytenum_prev, bytenum_new; // 8-byte int to allow 6-byte sequence number 

	//for zero-filling
	std::vector<char> zeros(packetsize, 0);

	//Where to save data (initialize with enough length)
	std::string filename("C:\\Users\\Youngjae\\Desktop\\YJ\\codes\\data_0.bin");
	std::ofstream output_file;
	std::remove(filename.c_str());		// clean previous data
	output_file.open(filename.c_str(), std::ofstream::in | std::ofstream::out | std::ofstream::app | std::ofstream::binary);
	if (output_file.is_open())
		printf("Board %u: Writing to file %s...\n", boardid, filename.c_str());
	else
	{
		printf("Board %u: Failed to open the file %s\n", boardid, filename.c_str());
		exit(EXIT_FAILURE);
	}

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	//server.sin_addr.s_addr = INADDR_ANY;
	server.sin_addr.s_addr = inet_addr(ADDR);
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	seqnum_prev = 0; // first packet's would be 1
	//bytenum_prev = 0;

	//keep listening for data
	while (1)
	{
		//printf("Waiting for data...\n");
		//fflush(stdout);

		//try to receive some data, this is a blocking call
		//if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR
		if ((recv_len = recvfrom(s, udpbuf, BUFLEN, 0, 0, 0)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		memcpy(&seqnum_new, udpbuf, 4);
		//memcpy(&bytenum_new, udpbuf + 4, 4); // should be corrected to accommodate 6-byte
		//printf("Received packet with seqnum:%d, bytenum:%d len:%d\n", seqnum_new, bytenum_new, recv_len);
		/*
		printf("Data:\n");
		for (int i = 0; i < recv_len; i++)
		{
		printf("%02x", (unsigned char)udpbuf[i]);
		}
		printf("\n");
		*/

		//discard old packet if it arrives
		if (seqnum_new <= seqnum_prev)
			continue;

		//zero-fill if there is dropped packets
		if (seqnum_new - seqnum_prev != 1)
		{
			//printf("Packet missed!! new : %d prev: %d Zero-filling..\n", seqnum_new, seqnum_prev);
			//printf("Current buffer's filled size is %d \n", buff_ptrs[0] - buff_begins[0]);
			//printf("byte new : %d byte prev :%d  filling %d \n", bytenum_new, bytenum_prev, (int)(bytenum_new - bytenum_prev));
			//std::fill(buff_ptrs[0], buff_ptrs[0] + (int)(bytenum_new - bytenum_prev), 0);
			for (int i = seqnum_prev + 1; i < seqnum_new; i++)
			{
				output_file.write(&*zeros.begin(), packetsize);
			}
			//printf("Zero-filling is done.\n");
		}


		//save this packet's data
		//std::copy(udpbuf, udpbuf + recv_len - 10, buff_ptrs[0]);
		//memcpy(buff_ptrs[0], udpbuf + 10, recv_len - 10);
		output_file.write(udpbuf+10, recv_len - 10);

		//update history
		seqnum_prev = seqnum_new;
		//bytenum_prev = bytenum_new + recv_len - 10;
		//printf("new byte prev is %d with %d + %d \n", bytenum_prev, bytenum_new, recv_len);
		//printf("Current sample #:%d\n", nsampcum);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}