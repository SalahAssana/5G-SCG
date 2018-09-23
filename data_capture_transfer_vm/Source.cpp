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
#define nTx 3
#define nRx 4
#define nSampperchirp 64
#define nLoop 1		//# of loops in a single frame
#define nframe2update 200
// assume each sample is complex with 16bit-I, 16bit-Q
// following xwr-14xx's order
// one chirp for each Tx  ex) chirp1 for Tx1, chirp2 for Tx2, 


int main()
{
	int nbytecum;
	std::ofstream lock, tmpfile;
	const std::string blockfile = "C:\\Users\\chony\\Desktop\\YJ\\codes\\blocked.txt";
	const std::string matlabsignal = "C:\\Users\\chony\\Desktop\\YJ\\codes\\reading.txt";
	int nbyte2update = 4 * nframe2update*nRx*nSampperchirp*nTx*nLoop;
	// buffer to save data : extra space for unmatched boundary and dropped packet (hopefully not many)
	std::vector<std::vector<char>> framebuffs(
		nBoard, std::vector<char>(nbyte2update)
		);
	//create a vector of pointers to point to each board's buffer
	std::vector<std::vector<char>::iterator> buff_begins;
	for (size_t i = 0; i < nBoard; i++) buff_begins.push_back(framebuffs[i].begin());
	//Where to save data (initialize with enough length)
	std::vector<std::string> readfilenames(nBoard, "C:\\Users\\chony\\Desktop\\YJ\\codes\\vm_share\\data_xx.bin");
	std::vector<std::string> writefilenames(nBoard, "C:\\Users\\chony\\Desktop\\YJ\\codes\\tmp_xx.bin");
	std::vector<std::ifstream> input_files(nBoard);
	for (int i = 0; i < nBoard; ++i) {
		sprintf(&readfilenames[i][0u], "C:\\Users\\chony\\Desktop\\YJ\\codes\\vm_share\\data_%d.bin", i);
		input_files[i].open(readfilenames[i].c_str(), std::ifstream::in | std::ifstream::out | std::ifstream::app | std::ifstream::binary);
		printf("Board %u: Reading from file %s...\n", i, readfilenames[i].c_str());
		sprintf(&writefilenames[i][0u], "C:\\Users\\chony\\Desktop\\YJ\\codes\\tmp_%d.bin", i);
		std::remove(writefilenames[i].c_str());		// clean previous data
		printf("Board %u: Writing to file %s...\n", i, writefilenames[i].c_str());
	}
	std::remove(blockfile.c_str());		// clean previous block file
	printf("bufferlength is %d \n", framebuffs[0].end() - framebuffs[0].begin());

	//keep listening for data
	while (1)
	{
		//printf("Reading data... \n"); 
		nbytecum = 0;	// cumulatively received bytes #
		//read data for the amount to be updated
		while (nbytecum < nbyte2update)
		{
			input_files[0].clear();
			nbytecum += (int)(input_files[0].read(&*buff_begins[0] + nbytecum, nbyte2update - nbytecum).gcount());
			//printf("Now %d bytes\n", nbytecum);
			Sleep(1);
		}

		//printf("Writing data to file...\n");

		//block MATLAB to write files
		lock.open(blockfile.c_str(), std::ofstream::binary);
		lock.close();

		//for the case MATLAB access the files while the blockfile is being created (yield)
		Sleep(3);
		while (_access(matlabsignal.c_str(), 0) == 0)
			Sleep(1);

		//output buff to files
		for (unsigned int i = 0; i < nBoard; ++i){
			std::ofstream output(writefilenames[i].c_str(), std::ofstream::binary | std::ofstream::app);
			std::copy(buff_begins[i], buff_begins[i] + nbyte2update, std::ostreambuf_iterator<char>(output));
			output.close();
		}
		// let MATLAB know the writing is over
		remove(blockfile.c_str());
		//("Done!\n");
	}

	return 0;
}