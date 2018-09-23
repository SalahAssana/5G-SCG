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

#define ADDR_host "192.168.33.30"	//Address of adapter used for communication
#define ADDR_board "192.168.33.180"
#define PORT 4096   //The port on which to listen for incoming data

int main()
{
	SOCKET s;
	struct sockaddr_in pc, board;
	int send_len;
	char udpbuf[26];
	WSADATA wsa;
	std::ofstream lock, tmpfile;
	//int bytenum_prev, bytenum_new; // 8-byte int to allow 6-byte sequence number 

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
	pc.sin_family = AF_INET;
	//server.sin_addr.s_addr = INADDR_ANY;
	pc.sin_addr.s_addr = inet_addr(ADDR_host);
	pc.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&pc, sizeof(pc)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//Prepare the sockaddr_in structure
	board.sin_family = AF_INET;
	//server.sin_addr.s_addr = INADDR_ANY;
	board.sin_addr.s_addr = inet_addr(ADDR_board);
	board.sin_port = htons(PORT);

	//Connect
	if (connect(s, (struct sockaddr *)&board, sizeof(board)) == SOCKET_ERROR)
	{
		printf("Connect failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Connect done");

	UINT16 header = 42330;  //0xA55A
	UINT16 cmd = 4;
	UINT16 size = 18;
	UINT8 sys_ip[4] = { 42, 33, 168, 192 };
	UINT8 fpga_ip[4] = { 180, 33, 168, 192 };
	UINT8 fpga_mac[6] = { 14, 90, 78, 56, 34, 12 };
	UINT16 config_port = 4096;
	UINT16 data_port = 4098;
	UINT16 footer = 61098;  //0xeeaa

	memcpy(udpbuf, &header, 2);
	memcpy(udpbuf + 2, &cmd, 2);
	memcpy(udpbuf + 4, &size, 2);
	memcpy(udpbuf + 6, sys_ip, 4);
	memcpy(udpbuf + 10, fpga_ip, 4);
	memcpy(udpbuf + 14, fpga_mac, 6);
	memcpy(udpbuf + 20, &config_port, 2);
	memcpy(udpbuf + 22, &data_port, 2);
	memcpy(udpbuf + 24, &footer, 2);

	printf("Data:\n");
	for (int i = 0; i < 26; i++)
	{
		printf("%02x", (unsigned char)udpbuf[i]);
	}
	printf("\n");

	// Send an initial buffer
	send_len = send(s, udpbuf, 26, 0);
	if (send_len == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	return 0;
}