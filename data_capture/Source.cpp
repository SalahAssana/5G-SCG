/*
Simple UDP Server
*/

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

#define BUFLEN 2048  //Max length of buffer
#define PORT 4098   //The port on which to listen for incoming data
#define nBoard 1
#define nTx 2
#define nRx 4
#define nSampperchirp 64
#define nLoop 1		//# of loops in a single frame
#define nframe2update 200
// assume each sample is complex with 16bit-I, 16bit-Q
// following xwr-14xx's order
// one chirp for each Tx  ex) chirp1 for Tx1, chirp2 for Tx2, 


int main()
{
	SOCKET s;
	struct sockaddr_in server;
	int recv_len, seqnum_prev, seqnum_new;
	char udpbuf[BUFLEN];
	WSADATA wsa;
	std::ofstream lock, tmpfile;
	const std::string blockfile = "C:\\Users\\chony\\Desktop\\YJ\\codes\\blocked.txt";
	const std::string matlabsignal = "C:\\Users\\chony\\Desktop\\YJ\\codes\\reading.txt";
	int bytenum_prev, bytenum_new; // 8-byte int to allow 6-byte sequence number 
	int nsampperframe = nRx*nSampperchirp*nTx*nLoop;
	// buffer to save data : extra space for unmatched boundary and dropped packet (hopefully not many)
	std::vector<std::vector<char>> framebuffs(
		nBoard, std::vector<char>(4*nsampperframe*nframe2update+200*BUFLEN)
		);
	//create a vector of pointers to point to each board's buffer
	std::vector<std::vector<char>::iterator> buff_begins, buff_ptrs;
	for (size_t i = 0; i < nBoard; i++) buff_begins.push_back(framebuffs[i].begin());
	buff_ptrs = buff_begins;
	//Where to save data (initialize with enough length)
	std::vector<std::string> filenames(nBoard, "C:\\Users\\chony\\Desktop\\YJ\\codes\\tmp_xx.bin");
	for (int i = 0; i < nBoard; ++i) {
		sprintf(&filenames[i][0u], "C:\\Users\\chony\\Desktop\\YJ\\codes\\tmp_%d.bin", i);
		printf("Board %u: Writing to file %s...\n", i, filenames[i].c_str());
		//std::cout << "Board " << i << ": Writing to file" << filenames[i].c_str() << "...\n";
		//filenames[i] = "tmp_" + boost::lexical_cast<std::string>(i)+".bin";
		//std::cout << boost::format("Board %u: Writing to file %s...\n") % i % filenames[i];
	}

	printf("bufferlength is %d \n", framebuffs[0].end() - framebuffs[0].begin());

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
	server.sin_addr.s_addr = inet_addr("192.168.33.30");
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	seqnum_prev = 0; // first packet's would be 1
	bytenum_prev = 0;

	int nsampcum = 0; // cumulatively received samples #
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
		memcpy(&bytenum_new, udpbuf + 4, 4); // should be corrected to accommodate 6-byte
		//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
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
			memset(&*buff_ptrs[0], 0, (int)(bytenum_new - bytenum_prev));
			buff_ptrs[0] += (int)(bytenum_new - bytenum_prev);
			nsampcum += (int)(bytenum_new - bytenum_prev) / 4;
			//printf("Zero-filling is done.\n");
		}
		
		
		//save this packet's data
		std::copy(udpbuf, udpbuf + recv_len - 10, buff_ptrs[0]);
		//memcpy(buff_ptrs[0], udpbuf + 10, recv_len - 10);
		buff_ptrs[0] += recv_len - 10;
		nsampcum += (recv_len - 10) / 4;
		
		//update history
		seqnum_prev = seqnum_new;
		bytenum_prev = bytenum_new + recv_len-10;
		//printf("new byte prev is %d with %d + %d \n", bytenum_prev, bytenum_new, recv_len);
		//printf("Current sample #:%d\n", nsampcum);
		if (nsampcum > nframe2update*nsampperframe)
		{
			nsampcum -= nframe2update*nsampperframe;
			//printf("Writing data to file...");
			
			//block MATLAB to write files
			lock.open(blockfile.c_str(), std::ofstream::binary);
			lock.close();

			//for the case MATLAB access the files while the blockfile is being created (yield)
			Sleep(3);
			while (_access(matlabsignal.c_str(), 0) == 0)
				Sleep(1);

			//output buff to files
			for (unsigned int i = 0; i < nBoard; ++i){
				/*
				tmpfile.open(filenames[i].c_str(), std::ofstream::binary);
				tmpfile.write((char*)buff_begins[i], 4*nframe2update*nsampperframe);
				tmpfile.close();
				*/
				std::ofstream output_file(filenames[i].c_str(), std::ofstream::binary | std::ofstream::app);
				//std::ostream_iterator<char> output_iterator(output_file, "\n");
				//std::copy(buff_begins[i], buff_begins[i] + 4 * nframe2update*nsampperframe, output_iterator);
				std::copy(buff_begins[i], buff_begins[i] + 4 * nframe2update*nsampperframe, std::ostreambuf_iterator<char>(output_file));
				output_file.close();
			}
			// let MATLAB know the writing is over
			remove(blockfile.c_str());
			//("Done!\n");

			// clean buffer
			for (unsigned int i = 0; i < nBoard; ++i){
				std::copy(buff_ptrs[i] - 4 * nsampcum, buff_ptrs[i], buff_begins[i]);
				//memcpy(buff_begins[i], buff_ptrs[i] - 4 * nsampcum, 4 * nsampcum);
				buff_ptrs[i] = buff_begins[i] + 4 * nsampcum;
			}
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}